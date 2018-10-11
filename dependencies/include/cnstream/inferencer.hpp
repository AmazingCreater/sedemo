/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAM_INFERENCER_HPP_
#define CNSTREAM_INFERENCER_HPP_

#include <string>
#include <thread>
#include <vector>

#include "cnstream_error.hpp"
#include "mrtcxx_types.h"
#include "module.hpp"

namespace cnstream {

class InferencerPrivate;

CNSTREAM_REGISTER_EXCEPTION(Inferencer)

class Inferencer : public Module {
 public:
  //  create a instance inferencer with default pram func_type
  static std::shared_ptr<Inferencer> Create(
      const std::string& model_path,
      const std::string& function_name,
      std::shared_ptr<Connector> input,
      const std::vector<mrtcxx::DataType>& vec_o_dtype,
      const std::vector<mrtcxx::DataOrder>& vec_i_data_order,
      const std::vector<mrtcxx::DataOrder>& vec_o_data_order,
      mrtcxx::MluMode func_type = mrtcxx::MLU_MODE_UNION2);
  ~Inferencer();
  const Inputs& inputs() const override;
  const Outputs& outputs() const override;

 protected:
  void Start() override;
  void Stop() override;
  void SetPerformanceDisplayMode(PerformanceDisplayMode mode) override;

 private:
  Inferencer();
  InferencerPrivate* d_ptr_;
  DECLARE_PRIVATE(d_ptr_, Inferencer)
};

}  // namespace cnstream
#endif  //  CNSTREAM_INFERENCER_HPP_
