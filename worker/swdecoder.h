#ifndef SWDECODER_H
#define SWDECODER_H
#include <cnstream/cnstream.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>

class SwDecoderError : public cnstream::CnstreamError {
 public:
  explicit SwDecoderError(std::string msg) :
    CnstreamError(msg) {}
};

class SwDecoder : public cnstream::Module
{
public:
    static std::shared_ptr<SwDecoder> Create(uint batch_size, uint dst_w, uint dst_h);
    void CheckAndInitialize() override;
    void Start() override;
    void Stop() override;
    void Destroy() override;

private:
    SwDecoder(const cnstream::Inputs &inputs, const cnstream::Outputs &outputs);
    void run(int channel_id);

private:
    std::vector<std::thread> threads_;
    uint cache_index_ = 0;
    uint frame_index_ = 0;
    std::shared_ptr<cnstream::Tensor> cache_tensor1_ = nullptr, cache_tensor2_;
    std::vector<std::shared_ptr<std::mutex>> buf_mutexs_;
};

#endif // SWDECODER_H
