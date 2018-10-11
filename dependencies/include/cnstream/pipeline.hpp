/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAM_PIPELINE_HPP_
#define CNSTREAM_PIPELINE_HPP_

#include <string>
#include <vector>
#include "module.hpp"

namespace cnstream {

class PipelinePrivate;

class Pipeline {
 public:
  Pipeline();
  explicit Pipeline(const std::vector<std::string>& input_videos);
  Pipeline(const Pipeline& other);
  Pipeline& operator=(const Pipeline& other);
  ~Pipeline();

  void AddModule(const std::shared_ptr<Module>& module_sptr);
  void Start();
  void Stop();
  void AddVideo(const std::string& video_path);
  const std::vector<std::string>& input_videos() const;
  uint32_t channel_count() const;
  void SetPerformanceDisplayMode(PerformanceDisplayMode mode);

 private:
  PipelinePrivate* d_ptr_;
  DECLARE_PRIVATE(d_ptr_, Pipeline);
};  // class Pipeline

}  // namespace cnstream

#endif  // CNSTREAM_PIPELINE_HPP_
