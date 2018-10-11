#include <cmath>
#include <opencv2/imgproc/imgproc.hpp>
#include "ssd_pre_proc.h"

using namespace cv;
using namespace std;
using namespace cnstream;

shared_ptr<SsdPreProc> SsdPreProc::Create(const Inputs& inputs, int mean_values[3], int o_w, int o_h, mrtcxx::DataType o_dtype) {
  CHECK_EQ(inputs.size(), 1);
  CHECK_EQ(inputs[0]->tensor_descs().size(), 1);
  CHECK_EQ(inputs[0]->tensor_descs()[0].type(), TensorType::RGB);
  CHECK_EQ(inputs[0]->tensor_descs()[0].dtype(), mrtcxx::DataType::UINT8);
  auto tensor_descs = inputs[0]->tensor_descs();
  tensor_descs[0].set_dtype(o_dtype);
  auto shape = tensor_descs[0].shape();
  shape.set_w(o_w);
  shape.set_h(o_h);
  tensor_descs[0].set_shape(shape);
  auto con = make_shared<Connector>(inputs[0]->buf_count(), tensor_descs);
  return shared_ptr<SsdPreProc>(new SsdPreProc(inputs, {con}, mean_values));
}

SsdPreProc::SsdPreProc(const Inputs& inputs, const Outputs& outputs, int mean_values[3]) :
    Module("detection pre proc", inputs, outputs) {
  SetMean(mean_values);
}

void SsdPreProc::Start() {
  threads_.reserve(inputs()[0]->buf_count());
  for (size_t i = 0; i < inputs()[0]->buf_count(); ++i) {
    threads_.push_back(move(thread(&SsdPreProc::ThreadFunc, this, i)));
  }
}

void SsdPreProc::Stop() {
  for (auto& it : threads_) {
    it.join();
  }
}

void SsdPreProc::ThreadFunc(int buf_id) {
  uint32_t batch_size = inputs()[0]->tensor_descs()[0].shape().n();
  auto o_dtype = outputs()[0]->tensor_descs()[0].dtype();
  auto i_shape = inputs()[0]->tensor_descs()[0].shape();
  auto o_shape = outputs()[0]->tensor_descs()[0].shape();
  try {
    while(running()) {
      GetInputData input(this, 0, buf_id);
      GetOutputBuf output(this, 0, buf_id);
      auto in_tensor = input[0];
      auto out_tensor = output[0];
      // set stream attr
      out_tensor->set_channel_id(in_tensor->channel_id());
      out_tensor->set_frame_ids(in_tensor->frame_ids());
      for (uint32_t batch = 0; batch < batch_size; ++batch) {
        auto in_batch_ptr = in_tensor->mutable_cpu_batch_data(batch);
        auto out_batch_ptr = out_tensor->mutable_cpu_batch_data(batch);
        Mat mat_in(i_shape.h(), i_shape.w(), CV_8UC3, in_batch_ptr);
        Mat mat_out;
        Mat mat_resize;
        Mat mat_convert;

        // out img
        switch (o_dtype) {
          case mrtcxx::DataType::UINT8:
            mat_out = Mat(o_shape.h(), o_shape.w(), CV_8UC3, out_batch_ptr);
            break;
          case mrtcxx::DataType::FLOAT32:
            mat_out = Mat(o_shape.h(), o_shape.w(), CV_32FC3, out_batch_ptr);
            break;
          default:
            LOG(FATAL) << "nonsupport";
            break;
        }

        // resize
        if (i_shape != o_shape) {
          cv::resize(mat_in, mat_resize, Size(o_shape.w(), o_shape.h()));
        } else {
          mat_resize = mat_in;
        }
        
        // convert data type
        switch (o_dtype) {
          case mrtcxx::DataType::UINT8:
            mat_convert = mat_resize;
            break;
          case mrtcxx::DataType::FLOAT32:
            mat_resize.convertTo(mat_convert, CV_32FC3);
            break;
          default:
            LOG(FATAL) << "nonsupport";
            break;
        }

        // reduction of the mean for float
        Mat norm;
        switch (o_dtype) {
          case mrtcxx::DataType::UINT8:
            norm = mat_convert;
            break;
          case mrtcxx::DataType::FLOAT32:
            subtract(mat_convert, mean_, norm);
            break;
          default:
            LOG(FATAL) << "nonsupport";
            break;
        }

        // layered
        vector<Mat> aryMat;
        for(uint32_t m=0; m < o_shape.c(); m++)
        {
          switch (o_dtype) {
          case mrtcxx::DataType::UINT8: {
              Mat t(o_shape.h(), o_shape.w(), CV_8UC1, mat_out.data + m * o_shape.h() * o_shape.w() * sizeof(uint8_t));
              aryMat.push_back(t);
            }
            break;
          case mrtcxx::DataType::FLOAT32: {
              Mat t(o_shape.h(), o_shape.w(), CV_32FC1, mat_out.data + m * o_shape.h() * o_shape.w() * sizeof(float));
              aryMat.push_back(t);
            }
            break;
          default:
            LOG(FATAL) << "nonsupport";
            break;
          }
            
        }
        cv::split(norm, aryMat);
      }
    }
  } catch (BufferStop& buf_stop) {
  }
}

void SsdPreProc::SetMean(int mean_values[3]) {
  if (nullptr == mean_values) return;
  auto shape = outputs()[0]->tensor_descs()[0].shape();
  std::vector<cv::Mat> channels;
  for (int i = 0; i < 3; ++i) {
    /* Extract an individual channel. */
    Mat channel(shape.h(), shape.w(), CV_32FC1,
        Scalar(mean_values[i]));
    channels.push_back(channel);
  }
  merge(channels, mean_);
}
