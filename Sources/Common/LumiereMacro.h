#pragma once
#include "LumiereConfig.h"
#include <string>
#include <iostream>
#include <fmt/format.h>

#define __FUNCTION_SITE__   __FUNCTION__, __FILE__, __LINE__
#define __CODE__SITE__      __FUNCTION__, __FILE__, __LINE__
#define __EXCEPTION_SITE__  __FILE__, __FUNCTION__, __LINE__


#define BEGIN_LUMIERE_NAMESPACE namespace NAMESPACE_NAME {
#define END_LUMIERE_NAMESPACE } // NAMESPACE_NAME


//#define LUMIERE_ENABLE_CUDA 1
#define LUMIERE_ENABLE_DEBUG 1
#define LUMIERE_FLOAT_AS_DOUBLE 1

#ifdef LUMIERE_ENABLE_CUDA
#define LUMIERE_IS_DEVICE_CODE
#endif


#if defined(LUMIERE_ENABLE_CUDA) && defined(__CUDACC__)
#define LUMIERE_HOST_DEVICE __host__ __device__
    #define LUMIERE_DEVICE __device__
#else
#define LUMIERE_HOST_DEVICE
#define LUMIERE_DEVICE
#endif


#ifdef LUMIERE_FLOAT_AS_DOUBLE
using Float = double;
#else
using Float = float;
#endif


#ifdef LUMIERE_FLOAT_AS_DOUBLE
using FloatBits = uint64_t;
#else
using FloatBits = uint32_t;
#endif
static_assert(sizeof(Float) == sizeof(FloatBits), "Float and FloatBits must have the same size");


#define LUMIERE_FORMAT(fmtStr, ...) fmt::format(fmtStr, __VA_ARGS__)