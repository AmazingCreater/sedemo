/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAM_CONNECTOR_HPP_
#define CNSTREAM_CONNECTOR_HPP_

#include <vector>
#include "tensor.hpp"

namespace cnstream {

class Buffer;
class Module;
class ModulePrivate;
class ConnectorPrivate;

class BufferStop {};

class Connector {
 public:
  friend class Buffer;
  friend class Module;
  friend class ModulePrivate;
  Connector(size_t buf_count,
           std::vector<TensorDesc> vec_tensor_desc,
           size_t max_buf_size = 20,
           size_t init_buf_size = 0);
  ~Connector();
  size_t buf_count() const;
  const std::vector<TensorDesc>& tensor_descs() const;

 private:
  // -----------------MEMBERS----------------- //
  ConnectorPrivate* d_ptr_;

  // ------------------MACRO------------------ //
  DECLARE_PRIVATE(d_ptr_, Connector);
  DISABLE_COPY_AND_ASSIGN(Connector);
};  // class Connector

typedef std::vector<std::shared_ptr<Connector>> Inputs;
typedef std::vector<std::shared_ptr<Connector>> Outputs;

}  // namespace cnstream

#endif  // CNSTREAM_CONNECTOR_HPP_
