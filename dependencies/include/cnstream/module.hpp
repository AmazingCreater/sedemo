/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAM_MODULE_HPP_
#define CNSTREAM_MODULE_HPP_

#include <chrono>
#include <string>
#include <vector>

#include "connector.hpp"

namespace cnstream {

class ModulePrivate;
class Pipeline;

enum class PerformanceDisplayMode {
  NONUSE = 0,
  AVG_TIME,
  DETAIL
};  // enum class PerformanceDisplayMode

class Module {
 public:
  friend class Pipeline;
  Module(Inputs inputs, Outputs outputs);
  Module(std::string name, Inputs inputs, Outputs outputs);
  virtual ~Module();

  std::string name() const;
  void set_name(std::string name);
  const Pipeline* container() const;
  virtual const Inputs& inputs() const;
  virtual const Outputs& outputs() const;
  /*
  size_t InputSize() const;
  size_t OutputSize() const;
  const std::vector<TensorDesc>& InputDescs(int input_id) const;
  const std::vector<TensorDesc>& OutputDescs(int output_id) const;
  size_t InputBufCount(int input_id) const;
  size_t OutputBufCount(int output_id) const;
  */
  bool running() const;

  struct GetInputData {
    GetInputData(Module* owner, int input_id, int buf_id) noexcept(false);
    std::shared_ptr<Tensor> data(int id);
    std::shared_ptr<Tensor> operator[](int id);
    void next_data() noexcept(false);
    ~GetInputData();
   private:
    Module* owner_; int input_id_; int buf_id_;
    std::vector<std::shared_ptr<Tensor>> data_;
    DISABLE_COPY_AND_ASSIGN(GetInputData);
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  };  // struct GetInputData

  struct GetOutputBuf {
    GetOutputBuf(Module* owner, int output_id, int buf_id) noexcept(false);
    std::shared_ptr<Tensor> buf(int id);
    std::shared_ptr<Tensor> operator[](int id);
    void next_buf() noexcept(false);
    ~GetOutputBuf();
   private:
    Module* owner_; int output_id_; int buf_id_;
    std::vector<std::shared_ptr<Tensor>> buf_;
    DISABLE_COPY_AND_ASSIGN(GetOutputBuf);
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
  };  // struct GetOutputBuf

 protected:
  virtual void CheckAndInitialize();
  virtual void Destroy();
  virtual void Start() = 0;
  virtual void Stop();
  virtual void SetPerformanceDisplayMode(PerformanceDisplayMode mode);

 private:
  ModulePrivate* d_ptr_;
  DECLARE_PRIVATE(d_ptr_, Module);
  DISABLE_COPY_AND_ASSIGN(Module);
};  // class Module

}  // namespace cnstream

#endif  // CNSTREAM_MODULE_HPP_
