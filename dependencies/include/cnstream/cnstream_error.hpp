/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAM_CNSTREAM_ERROR_HPP_
#define CNSTREAM_CNSTREAM_ERROR_HPP_

#include <stdexcept>
#include <string>

#define CNSTREAM_REGISTER_EXCEPTION(CNAME)                \
class CNAME##Error : public CnstreamError {      \
 public:                                         \
  explicit CNAME##Error(std::string msg) :       \
    CnstreamError(msg) {}                        \
};

namespace cnstream {

class CnstreamError : public std::runtime_error {
 public:
  explicit CnstreamError(std::string msg) :
      std::runtime_error(msg) {}
};  // class CnstreamError

}  // namespace cnstream

#endif  // CNSTREAM_CNSTREAM_ERROR_HPP_
