#include <cassert>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <glog/logging.h>
#include <cnstream/pipeline.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QThread>
#include "detection_post_proc.h"
#include "displayer.h"
#include "cnstreamworker.h"
static const int g_bw = 800, g_bh = 1280;

class BigImageWorker {
 public:
  void init() {
    th_ = std::thread(&BigImageWorker::run, this);
  }
  void run() {
      while (running_) {
        std::unique_lock<std::mutex> lock(mux_);
        if (working_) {
            task_();
            working_ = false;
        } else {
            sync_.wait(lock);
        }
        done_.notify_one();
      }
  }
  void work(const std::function<void()> &task) {
    std::unique_lock<std::mutex> lock(mux_);
    working_ = true;
    task_ = task;
    sync_.notify_one();
  }
  void stop() {
    std::unique_lock<std::mutex> lock(mux_);
    running_ = false;
    sync_.notify_one();
    lock.unlock();
    th_.join();
  }
  void sync() {
    std::unique_lock<std::mutex> lock(mux_);
    if (working_)
      done_.wait(lock);
  }
 private:
  std::thread th_;
  std::function<void()> task_;
  bool running_ = true;
  bool working_ = false;
  std::mutex mux_;
  std::condition_variable sync_;
  std::condition_variable done_;
} g_image_worker[64];  // class BigImageWorker

std::shared_ptr<Displayer> Displayer::Create(int frame_rate,
    PostProcessing postcb,
    CnstreamWorker *worker,
    const cnstream::Inputs& inputs) {
  auto pdisp = std::shared_ptr<Displayer>(new Displayer(inputs, {}));
  pdisp->postcb_ = postcb;
  pdisp->frame_rate_ = frame_rate;
  pdisp->worker_ = worker;
  return pdisp;
}

Displayer::Displayer(const cnstream::Inputs &inputs,
    const cnstream::Outputs &outputs) :
    QObject(nullptr), Module("Displayer", inputs, outputs), timer_(this) {
}

void Displayer::CheckAndInitialize() {
  assert(inputs().size() == 2);  // one for inferencer output, one for images.
  assert(inputs()[0]->buf_count() == inputs()[1]->buf_count());

  // init buffers
  uint32_t channel_count = container()->channel_count();
  vec_image_buf_list_.resize(channel_count);
  vec_total_size_.resize(channel_count, 0);
  vec_erase_size_.resize(channel_count, 0);
  cache_frames_.resize(channel_count);
  for (int i = 0; i <  channel_count; ++i) {
    track_channels_.push_back(std::make_shared<MotFilter>());
  }

  big_img_ = cv::Mat(g_bh, g_bw, CV_8UC3);
  // connect to timer
  connect(&timer_, SIGNAL(timeout()), this, SLOT(HandleTimeOut()));
//  connect(this, SIGNAL(start_timer(int)), &timer_, SLOT(start(int)));
  connect(this, SIGNAL(stop_timer()), &timer_, SLOT(stop()));

  // init image workers
  for (uint i = 0; i < channel_count; ++i) {
    g_image_worker[i].init();
  }
}

void Displayer::Start() {
  size_t buf_count = inputs()[0]->buf_count();
  for (size_t i = 0; i < buf_count; ++i) {
    threads_.push_back(std::thread(&Displayer::ThreadFunc, this, i));
  }
  auto delay = 1000 / frame_rate_;
  timer_.start(delay);
  this->moveToThread(&timer_th_);
  timer_th_.start();
}

void Displayer::Stop() {
  emit stop_timer();
  timer_th_.quit();
  for (auto& it : threads_) {
    it.join();
  }
}

void Displayer::Destroy() {
    uint32_t channel_count = container()->channel_count();
    // stop image workers
    for (uint i = 0; i < channel_count; ++i) {
      g_image_worker[i].stop();
    }
}
namespace detection {
  extern std::vector<std::string> g_labels;
}
void Displayer::ThreadFunc(int buf_id) {
  // nn --->> neural network
  int nn_output_nm = inputs()[1]->tensor_descs().size();
  try {
    while (running()) {
      GetInputData img_getter(this, 0, buf_id);
      GetInputData nn_output_getter(this, 1, buf_id);
      auto image_tensor = img_getter.data(0);
      //  nn output tensors
      std::vector<std::shared_ptr<cnstream::Tensor>> nn_output_tensors;
      for (int i = 0; i < nn_output_nm; ++i) {
        nn_output_tensors.push_back(nn_output_getter[i]);
      }
      auto image_shape = image_tensor->shape();
      uint32_t channel_id = image_tensor->channel_id();
#ifdef _DEBUG
      DLOG(INFO) << "buf_id :" << buf_id << "image channel id:" << channel_id
          << " nn output channel id:" << nn_output_tensor->channel_id();
      CHECK_EQ(channel_id, nn_output_tensors[0]->channel_id());
      auto image_frame_ids = image_tensor->frame_ids();
      auto nn_output_frame_ids = nn_output_tensors[0]->frame_ids();
      LOG_IF(FATAL, image_tensor->frame_ids() != nn_output_tensor->frame_ids()) << "frame ids not equal:";
#endif  // _DEBUG

      auto image_frame_ids = image_tensor->frame_ids();

      // handle image loop
      uint32_t batch_size = image_shape.n();
      for (uint32_t n = 0; n < batch_size; ++n) {

        uint64_t frame_id = static_cast<uint64_t>(image_frame_ids[n]);
        // get one batch image data ptr
        uint8_t* image_data = static_cast<uint8_t*>(image_tensor->mutable_cpu_batch_data(n));
        // get one batch nn output data ptrs
        std::vector<float*> nn_output_ptrs;
        for (auto it : nn_output_tensors) {
          auto tptr = reinterpret_cast<float*>(it->mutable_cpu_data()
              ) + it->shape().nhwc() / batch_size * n;
          nn_output_ptrs.push_back(tptr);
        }
        // create image mat
        cv::Mat matt(image_shape.h(), image_shape.w(), CV_8UC3, image_data);
        cv::Mat image_origin = matt.clone();
        auto image_after_postprocess = image_origin.clone();
        // post process
        if (postcb_ != nullptr) {
          // get one batch shape for nn outputs.
          std::vector<mrtcxx::Shape> nn_shapes;
          for (auto it : nn_output_tensors) {
            auto tshape = it->shape();
            tshape.set_n(tshape.n() / batch_size);
            nn_shapes.push_back(tshape);
          }
          DS_DetectObjects detect_objects;
          (*postcb_)(image_after_postprocess, detect_objects, nn_output_ptrs, nn_shapes);
          // track
          TrackAndObjectOutput(image_origin, detect_objects, frame_id, channel_id);
        }
        // update buffer
        vec_image_buf_list_[channel_id].push_back(std::move(image_after_postprocess));
        vec_total_size_[channel_id]++;
      }
    }
  } catch (cnstream::BufferStop& buf_stop) {
    DLOG(INFO) << "stopped";
  }
}
#include <chrono>
#include <fstream>

class TimeCnter {
 public:
    void start() {
        last = std::chrono::high_resolution_clock::now();
    }
    void dot(std::string head) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = now - last;
        std::string str = head + ":" + std::to_string(diff.count()) + "ms\n";
        std::cout << str;
        last = now;
    }

 std::chrono::time_point<std::chrono::system_clock> last;
};  // class TimeCnter

void Displayer::HandleTimeOut() {
  // update image
    TimeCnter tcnter;
    tcnter.start();
  int chn_w = g_bw / 4, chn_h = g_bh / 8;
  int channel_nm = vec_image_buf_list_.size();
  // sync
  for (int i = 0; i < channel_nm; ++i) {
      if (vec_total_size_[i] == vec_erase_size_[i]) {
          return;
      }
  }
  for (int i = 0 ; i < channel_nm; ++i) {
//    if (vec_total_size_[i] == vec_erase_size_[i]) {
//      // no image update in this channel
//      // continue;
//        return;
//    }

    std::function<void()> task = [this, chn_w, chn_h, i]() {
        auto image = vec_image_buf_list_[i].front();
        vec_image_buf_list_[i].pop_front();
        vec_erase_size_[i]++;
        // selected
        if (i == selected_chn_) {
            cv::Mat rgb;
            cv::cvtColor(image, rgb, CV_BGR2RGB);
            emit worker_->sig_refresh_detail(rgb);
        }
        // copy to big image
        cv::Mat resize;
        cv::resize(image, resize, cv::Size(chn_w, chn_h));
        auto roi = big_img_(cv::Rect(chn_w * (i % 4), i / 4 * chn_h, chn_w, chn_h));
        resize.copyTo(roi);
    };
    g_image_worker[i].work(task);
  }
  for (int i = 0; i < channel_nm; ++i) {
      g_image_worker[i].sync();
  }
  tcnter.dot("copy to big image");
  cv::Mat rgb;
  cv::cvtColor(big_img_, rgb, CV_BGR2RGB);
  emit worker_->sig_refresh(rgb);
  tcnter.dot("bgr to rgb");
}

void Displayer::ChangeChn(int chn) {
    selected_chn_ = chn;
}

void Displayer::TrackAndObjectOutput(cv::Mat& image_origin, const DS_DetectObjects &detect_objects, const uint64_t &frame_id, const uint32_t &channel_id)
{
    auto pmot_filter = track_channels_[channel_id];
    std::vector<MotObject> shot_objects;
    bool cache_frame;
    bool release_frame;
    uint64_t release_frame_id;

    pmot_filter->Update(detect_objects, frame_id, shot_objects, cache_frame, release_frame, release_frame_id);

    if(cache_frame){
        cache_frames_[channel_id].insert(std::pair<uint64_t, cv::Mat>(frame_id, image_origin));
        cache_frame_count_ ++;
    }

    for(auto& shot_object:shot_objects)
    {
        std::map<uint64_t, cv::Mat>::iterator iter;
        cv::Mat object_frame;
        iter = cache_frames_[channel_id].find(shot_object.frame_id);
        if(iter != cache_frames_[channel_id].end())
        {
            object_frame = iter->second;
        }
        else
        {
            continue;
        }
        if(shot_object.x < 0) shot_object.x = 0;
        if(shot_object.y < 0) shot_object.y = 0;
        if(shot_object.width < 0) shot_object.width = 0;
        if(shot_object.height < 0) shot_object.height = 0;
        if(shot_object.x + shot_object.width > object_frame.cols) shot_object.width = object_frame.cols -shot_object.x;
        if(shot_object.y + shot_object.height > object_frame.rows) shot_object.height = object_frame.rows -shot_object.y;
        cv::Rect cut_rect(shot_object.x,shot_object.y,shot_object.width,shot_object.height);
        cv::Mat cut_img=object_frame(cut_rect);
        std::string object_class = detection::g_labels[shot_object.class_id];
        char msg[20];
        snprintf(msg, sizeof(msg), "%s", object_class.c_str());
        char *object_msg = msg;
        //TODO: objs
//        if(objcb_ != nullptr)
//        {
//          (*objcb_)(channel_id, cut_img, object_msg);
//        }
    } //  for(auto& shot_object:shot_objects)

    if(release_frame){
        std::map<uint64_t, cv::Mat>::iterator iter_release;
        iter_release = cache_frames_[channel_id].find(release_frame_id);
        if(iter_release != cache_frames_[channel_id].end())
        {
            cache_frames_[channel_id].erase(iter_release);
            cache_frame_count_ --;
        }
    } //  if(release_frame)
}
