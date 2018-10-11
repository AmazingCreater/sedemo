/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAM_TENSOR_HPP_
#define CNSTREAM_TENSOR_HPP_

#include <unistd.h>

#include <memory>
#include <string>
#include <vector>

#include "mrtcxx_types.h"
#include "base.hpp"

namespace cnstream {

enum class TensorType {
  RAW = 0,
  YUV,
  RGB,
  NEURON,
  COMMON
};  // enum class TensorType

std::ostream& operator<<(std::ostream& os, const TensorType& type);

std::ostream& operator<<(std::ostream& os, const mrtcxx::DataType& dtype);

/*****************************************************
 * @brief describe a tensor
 *****************************************************/
class TensorDesc {
 public:
  // ----------------CONSTRUCTOR-------------- //
  TensorDesc();
  TensorDesc(const mrtcxx::Shape& shape, TensorType type,
      mrtcxx::DataType dtype);

  // -----------------ACCESSOR---------------- //
  void set_shape(const mrtcxx::Shape& shape);
  void set_type(TensorType type);
  void set_dtype(mrtcxx::DataType d_type);
  const mrtcxx::Shape& shape() const;
  TensorType type() const;
  mrtcxx::DataType dtype() const;

  size_t UnitSize() const;
  size_t DataCount() const;
  std::string TypeStr() const {
    switch (type_) {
      case TensorType::RAW: return "RAW";
      case TensorType::YUV: return "YUV";
      case TensorType::RGB: return "RGB";
      case TensorType::NEURON: return "NEURON";
      case TensorType::COMMON: return "COMMON";
      default: return "UNKNOWN";
    }
  }
  std::string DtypeStr() const {
    switch (dtype_) {
      case mrtcxx::DataType::UINT8: return "UINT8";
      case mrtcxx::DataType::INT32: return "INT32";
      case mrtcxx::DataType::UINT32: return "UINT32";
      case mrtcxx::DataType::FLOAT32: return "FLOAT32";
      case mrtcxx::DataType::INT64: return "INT64";
      default: return "UNKNOWN";
    }
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  const TensorDesc &tensor_desc);

 private:
  // -----------------MEMBERS----------------- //
  mrtcxx::Shape shape_;
  TensorType type_;
  mrtcxx::DataType dtype_;
};  // class TensorDesc


class TensorPrivate;

class Tensor {
 public:
  /*****************************************************
   * @brief head
   *****************************************************/
  enum SyncedHead {
    kUNINITIALIZED = 0,
    kHEAD_AT_CPU,
    kHEAD_AT_MLU,
    kSYNCED
  };  // enum SyncedHead

  // ----------------CONSTRUCTOR-------------- //

  Tensor();
  explicit Tensor(const TensorDesc& tensor_desc);
  Tensor(uint32_t channel_id, const TensorDesc& tensor_desc);
  ~Tensor();

  // -----------------ACCESSOR---------------- //

  SyncedHead head() const;
  const TensorDesc& tensor_desc() const;
  const mrtcxx::Shape& shape() const;
  TensorType type() const;
  mrtcxx::DataType dtype() const;
  size_t capacity() const;
  uint32_t channel_id() const;
  const std::vector<uint64_t>& frame_ids() const;
  const void* cpu_data();
  const void* mlu_data();
  const void* cpu_batch_data(uint32_t batch_index);
  void* mutable_cpu_data();
  void* mutable_mlu_data();

  /*****************************************************
   * @brief get the mutable data pointer of one batch on
   * the cpu
   * @param
   *    batch_index: index of batch
   * @return
   *    return the mutable data pointer of one batch on
   *    the cpu
   *****************************************************/
  void* mutable_cpu_batch_data(uint32_t batch_index);

  void set_channel_id(uint32_t channel_id);
  void set_frame_ids(const std::vector<uint64_t>& frame_ids);
  void AppendFrameId(uint64_t frame_id);

  size_t UnitSize() const;
  /*****************************************************
   * @brief copy data from cpu
   * @param
   *    data: the pointer of data on the cpu
   * @return
   *    void
   * @attention
   *    ensure the size of data which stored in the
   *    pointer equals the count of data described by
   *    the shape of tensor
   *****************************************************/
  void CopyFromCpu(const void* data);
  /*****************************************************
   * @brief copy one batch of data from cpu
   * @param
   *    data: the pointer of data on the cpu
   *    batch_index: index of batch
   * @return
   *    void
   *****************************************************/
  void CopyBatchFromCpu(const void* data, uint32_t batch_index);
  /*****************************************************
   * @brief nonsupport
   *****************************************************/
  void CopyFromMlu(const void* data);
  /*****************************************************
   * @brief copy from tensor
   * @param
   *    src: source
   *****************************************************/
  void CopyFromTensor(const std::shared_ptr<Tensor>& src);
  /*****************************************************
   * @brief reshape
   * @param
   *    shape: shape
   *    type: tensor type
   * @return
   *    void
   *****************************************************/
  void Reshape(mrtcxx::Shape shape);

  /*****************************************************
   * @brief change the data type
   * @param
   *    d_type: data type
   *    bTransformData: convert data based on data type
   * @return
   *    void
   *****************************************************/
  // void ChangeDataType(const DataType &d_type, bool bTransformData);
  /*****************************************************
   * @brief save data to file
   * @param
   *    file path
   *****************************************************/
  // void Save(std::string fname);
  /*****************************************************
   * @brief print each data in tensor
   *****************************************************/
  // friend std::ostream &operator<<(std::ostream &os, const Tensor &tensor);

  void DumpData(std::string fname);
  void DumpData(std::ostream& os);
  friend std::ostream &operator<<(std::ostream &os, const Tensor &tensor);

 private:
  TensorPrivate* d_ptr_;

  DECLARE_PRIVATE(d_ptr_, Tensor);
  DISABLE_COPY_AND_ASSIGN(Tensor);
};  // class Tensor

}  // namespace cnstream

#endif  // CNSTREAM_TENSOR_HPP_
