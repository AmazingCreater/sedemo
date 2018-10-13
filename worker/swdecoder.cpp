#include <chrono>
#include <unistd.h>
#include "swdecoder.h"

class FrameRateController {
public:
    explicit FrameRateController(uint frame_rate) {
        delay_ = 1000000 / frame_rate;
    }
    void start() {
        begin_ = std::chrono::high_resolution_clock::now();
    }
    void end() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> diff = end - begin_;
        long sleep_time = delay_ - diff.count();
        if (sleep_time > 0) {
            usleep(sleep_time);
        }
    }
private:
    uint delay_;
    std::chrono::time_point<std::chrono::system_clock> begin_;
};  // class FrameRateController

SwDecoder::SwDecoder(const cnstream::Inputs &inputs, const cnstream::Outputs &outputs) : cnstream::Module("software decoder", inputs, outputs)
{

}

void SwDecoder::run(int channel_id)
{
    FrameRateController fps_controller(30);
    auto buf_cnt = outputs()[0]->buf_count();
    auto buf_id = channel_id % buf_cnt;
    auto img_shape = outputs()[0]->tensor_descs()[0].shape();
    // open video capture
    auto video_path = container()->input_videos()[static_cast<uint>(channel_id)];
    cv::VideoCapture capture;
    if (!capture.open(video_path)) {
        throw SwDecoderError("open video file failed!");
    }
    // create image buffer
    cv::Mat img(img_shape.h(), img_shape.w(), CV_8UC3);
    try {
        // output getter
        std::shared_ptr<cnstream::Module::GetOutputBuf> output_getter1 = std::make_shared<cnstream::Module::GetOutputBuf>(this, 0, buf_id);
        std::shared_ptr<cnstream::Module::GetOutputBuf> output_getter2 = std::make_shared<cnstream::Module::GetOutputBuf>(this, 1, buf_id);
        cache_tensor1_ = output_getter1->buf(0);
        cache_tensor1_->set_channel_id(channel_id);
        cache_tensor2_ = output_getter2->buf(0);
        // run
        while (running()) {
            fps_controller.start();
            cv::Mat t;
            if (!capture.read(t)) break;
            cv::resize(t, img, img.size());
            cache_tensor1_->CopyBatchFromCpu(img.data, cache_index_);
            cache_tensor1_->AppendFrameId(frame_index_);
            cache_index_ = (cache_index_ + 1) % img_shape.n();
            frame_index_++;
            if (0 == cache_index_) {
                cache_tensor2_->CopyFromTensor(cache_tensor1_);
                // sync buffer pipe
                buf_mutexs_[buf_id]->lock();
                output_getter1->next_buf();
                output_getter2->next_buf();
                buf_mutexs_[buf_id]->unlock();
                // new tensor
                cache_tensor1_ = output_getter1->buf(0);
                cache_tensor2_ = output_getter2->buf(0);
            }
            fps_controller.end();
        }
    } catch (cnstream::BufferStop &buf_stop) {
    }
    capture.release();
}

std::shared_ptr<SwDecoder> SwDecoder::Create(uint batch_size, uint dst_w, uint dst_h)
{
    cnstream::TensorDesc desc;
    desc.set_dtype(mrtcxx::UINT8);
    desc.set_shape(mrtcxx::Shape(batch_size, dst_h, dst_w, 3));
    desc.set_type(cnstream::TensorType::RGB);
    auto vec_desc = {desc};
    int buf_cnt = 32 / batch_size;
    auto output_con = std::make_shared<cnstream::Connector>(buf_cnt, vec_desc);
    auto decoder = std::shared_ptr<SwDecoder>(new SwDecoder({}, {output_con, output_con}));
    return decoder;
}

void SwDecoder::CheckAndInitialize()
{
    auto buf_cnt = outputs()[0]->buf_count();
    for (uint i = 0; i < buf_cnt; ++i) {
        buf_mutexs_.push_back(std::shared_ptr<std::mutex>(new std::mutex));
    }
}

void SwDecoder::Start()
{
    auto chn_cnt = container()->channel_count();
    for (uint i = 0; i < chn_cnt; ++i) {
        threads_.push_back(std::thread(&SwDecoder::run, this, i));
    }
}

void SwDecoder::Stop()
{
    for (auto &it : threads_) {
        it.join();
    }
}

void SwDecoder::Destroy()
{

}
