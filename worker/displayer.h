#ifndef DISPLAYER_H_
#define DISPLAYER_H_

#include <thread>
#include <QTimer>
#include <QThread>
#include <QVector>
#include <opencv2/core/core.hpp>
#include <cnstream/mrtcxx_types.h>
#include <cnstream/module.hpp>
#include <deepsort/deepsort.h>
#include "mot_filter.h"
#include "fpscal.h"

typedef void (*PostProcessing)(cv::Mat& img,
    DS_DetectObjects &detect_objects,
    const std::vector<float*> &nn_output_ptrs,
    const std::vector<mrtcxx::Shape> &nn_output_shapes);
class CnstreamWorker;
class Displayer : public QObject, public cnstream::Module {
 Q_OBJECT
 public:
  static std::shared_ptr<Displayer> Create(int frame_rate,
      PostProcessing postcb,
      CnstreamWorker *worker,
      const cnstream::Inputs& inputs);
 protected:
  void CheckAndInitialize() override;
  void Start() override;
  void Stop() override;
  void Destroy() override;

 public slots:
  void HandleTimeOut();
  void ChangeChn(int chn);

 Q_SIGNALS:
  void start_timer(int delay);
  void stop_timer();

 private:
  Displayer(const cnstream::Inputs &inputs, const cnstream::Outputs &outputs);
  void ThreadFunc(int buf_id);
  bool CacheComplete();
  void UpdateFps(int channel_id);
  void TempFps(int channel_id);
  QVector<cv::Mat> TrackAndObjectOutput(cv::Mat& image_origin, const DS_DetectObjects &detect_objects, const uint64_t &frame_id, const uint32_t &channel_id);
  void DrawDetectResult(cv::Mat& image, DS_DetectObjects &detect_objects, const uint32_t &channel_id);
  QTimer timer_;
  QThread timer_th_;
  int frame_rate_;
  const int kbstep_ = 8;
  std::vector<FpsCal> vec_fps_cals_;
  std::vector<std::list<cv::Mat>> vec_image_buf_list_;
  std::vector<std::list<QVector<cv::Mat>>> vec_objs_buf_list_;
  std::vector<std::list<int>> vec_fps_buf_list_;
  std::vector<size_t> vec_total_size_;
  std::vector<size_t> vec_erase_size_;
  std::vector<int> vec_last_cal_bindex_;
  std::vector<int> vec_last_update_bindex_;
  std::vector<int> vec_fps_for_show_;
  std::vector<std::shared_ptr<MotFilter>> track_channels_;
  std::vector<std::map<uint64_t, cv::Mat>> cache_frames_;
  int cache_frame_count_ = 0;
  std::vector<std::thread> threads_;
  PostProcessing postcb_ = nullptr;
  cv::Mat big_img_;
  int selected_chn_ = 0;
  CnstreamWorker *worker_;
  std::mutex data_mutex_;
};  // class Displayer
#endif  // DISPLAYER_H_

