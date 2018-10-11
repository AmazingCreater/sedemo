#ifndef SSD_PRE_PROC_H_
#define SSD_PRE_PROC_H_

#include <thread>
#include <opencv2/core/core.hpp>
#include <cnstream/mrtcxx_types.h>
#include <cnstream/module.hpp>

class SsdPreProc : public cnstream::Module {
 public:
  static std::shared_ptr<SsdPreProc> Create(const cnstream::Inputs& inputs, int mean_values[3], int o_w, int o_h, mrtcxx::DataType o_dtype);
  void Start() override;
  void Stop() override;
 private:
  SsdPreProc(const cnstream::Inputs& inputs, const cnstream::Outputs& outputs, int mean_values[3]);
  void ThreadFunc(int buf_id);
  void SetMean(int mean_values[3]);
  std::vector<std::thread> threads_;
  cv::Mat mean_;
}; // class SsdPreProc

#endif // SSD_PRE_PROC_H_
