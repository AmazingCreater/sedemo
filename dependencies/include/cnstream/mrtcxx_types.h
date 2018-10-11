/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef API_MRTCXX_INCLUDE_MRTCXX_TYPES_H_
#define API_MRTCXX_INCLUDE_MRTCXX_TYPES_H_

#include <iostream>
#include "cnrt.h"

namespace mrtcxx {

enum MluMode {
  MLU_MODE_UNION2 = CNRT_FUNC_TYPE_UNION2,
  MLU_MODE_BLOCK = CNRT_FUNC_TYPE_BLOCK
};

enum MluChannelId {
  CHANNEL_ID_NONE = CNRT_CHANNEL_TYPE_NONE,
  CHANNEL_ID_0 = CNRT_CHANNEL_TYPE_0,
  CHANNEL_ID_1 = CNRT_CHANNEL_TYPE_1,
  CHANNEL_ID_2 = CNRT_CHANNEL_TYPE_2,
  CHANNEL_ID_3 = CNRT_CHANNEL_TYPE_3
};

enum DataType {
  UINT8 = CNRT_UINT8,
  FLOAT32 = CNRT_FLOAT32,
  UINT32 = CNRT_UINT32,
  INT32 = CNRT_INT32,
  INT64 = CNRT_INVALID
};

enum DataOrder {
  NCHW = CNRT_NCHW,
  NHWC = CNRT_NHWC
};

/*****************************************************
 * @brief NHWC
 *****************************************************/
class Shape {
 public:
  explicit Shape(uint32_t n = 1, uint32_t h = 1,
                uint32_t w = 1, uint32_t c = 1);

  void set_n(uint32_t n);
  void set_h(uint32_t h);
  void set_w(uint32_t w);
  void set_c(uint32_t c);
  uint32_t n() const;
  uint32_t h() const;
  uint32_t w() const;
  uint32_t c() const;

  /********************************
   * @brief n * h * w * c
   ********************************/
  uint64_t nhwc() const;
  /********************************
   * @brief h * w * c
   ********************************/
  uint64_t hwc() const;
  /********************************
   * @brief h * w
   ********************************/
  uint64_t hw() const;

  friend std::ostream &operator<<(std::ostream &os, const Shape &shape);
  bool operator==(const Shape& other);
  bool operator!=(const Shape& other);

 private:
  // -----------------MEMBERS----------------- //
  uint32_t n_;
  uint32_t h_;
  uint32_t w_;
  uint32_t c_;
};  // class Shape

}  // namespace mrtcxx

#endif  // API_MRTCXX_INCLUDE_MRTCXX_TYPES_H_
