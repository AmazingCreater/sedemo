/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CORE_INCLUDE_PUBLIC_BASE_HPP_
#define CORE_INCLUDE_PUBLIC_BASE_HPP_

#include <unistd.h>

#include <glog/logging.h>

#define DISABLE_COPY_AND_ASSIGN(TypeName)                    \
  TypeName(const TypeName&) = delete;                        \
  const TypeName& operator=(const TypeName&) = delete;

#define CORE_NUM_PER_CHANNEL 8
#define CNRT_CHANNEL_NUM 4

#define DECLARE_PRIVATE(Dptr, Class)                              \
  inline Class##Private* d_func() {                               \
    return reinterpret_cast<Class##Private *>(Dptr);              \
  }                                                               \
  inline const Class##Private* d_func() const {                   \
    return reinterpret_cast<const Class##Private *>(Dptr);        \
  }                                                               \
  friend class Class##Private;

#define DECLARE_PUBLIC(Qptr, Class)                               \
  inline Class* q_func() {                                        \
    return static_cast<Class *>(Qptr);                            \
  }                                                               \
  inline const Class* q_func() const {                            \
    return static_cast<const Class*>(Qptr);                       \
  }                                                               \
  friend class Class;

#define D_D(Class) auto d_ptr = d_func();
#define D_Q(Class) auto q_ptr = q_func();

#define NONSUPPORTED LOG(FATAL) << "nonsupported.";

#define DEFAULT_ABORT LOG(FATAL) << "default."

#define CHECK_CNRT(RET)                            \
  LOG_IF(FATAL, CNRT_RET_SUCCESS != (RET))         \
    << " "

#endif  // CORE_INCLUDE_PUBLIC_BASE_HPP_
