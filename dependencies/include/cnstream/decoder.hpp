/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAM_DECODER_HPP_
#define CNSTREAM_DECODER_HPP_

#include "cnstream_error.hpp"
#include "module.hpp"

namespace cnstream {

class DecoderPrivate;

CNSTREAM_REGISTER_EXCEPTION(Decoder);

class Decoder : public Module {
 public:
  struct Attr {
    uint32_t src_frame_rate;
    uint32_t dst_frame_rate;
    uint32_t target_w;
    uint32_t target_h;
    bool input_image = false;
    bool roi_enable = false;
    float roi_xr = 0, roi_yr = 0;
    float roi_wr = 0, roi_hr = 0;
  };  // struct DecoderAttr
  static std::shared_ptr<Decoder> Create(Attr attr,
                                  uint32_t output_count,
                                  int batch_size = 8);
  ~Decoder();
  void CheckAndInitialize() override;
  void Destroy() override;
  void Start() override;
  void Stop() override;
  Attr attr() const;

 private:
  Decoder(Attr attr, const Outputs& outputs);
  DecoderPrivate* d_ptr_;
  DECLARE_PRIVATE(d_ptr_, Decoder);
};  // class Decoder

}  // namespace cnstream

#endif  // CNSTREAM_DECODER_HPP_
