#pragma once
#include <limits>
#include <cmath>
#include "Common/LumiereMacro.h"

#if defined(LUMIERE_IS_DEVICE_CODE)
#include <cuda_fp16.h>
#endif

BEGIN_LUMIERE_NAMESPACE

#ifdef LUMIERE_IS_DEVICE_CODE

#define DoubleOneMinusEpsilon 0x1.fffffffffffffp-1
#define FloatOneMinusEpsilon float(0x1.fffffep-1)

#ifdef LUMIERE_FLOAT_AS_DOUBLE
#define OneMinusEpsilon DoubleOneMinusEpsilon
#else
#define OneMinusEpsilon FloatOneMinusEpsilon
#endif

#define Infinity std::numeric_limits<Float>::infinity()
#define MachineEpsilon std::numeric_limits<Float>::epsilon() * 0.5f

#else

static constexpr double DoubleOneMinusEpsilon = 0x1.fffffffffffffp-1;
static constexpr float FloatOneMinusEpsilon = 0x1.fffffep-1;

#ifdef LUMIERE_FLOAT_AS_DOUBLE
static constexpr double OneMinusEpsilon = DoubleOneMinusEpsilon;
#else
static constexpr float OneMinusEpsilon = FloatOneMinusEpsilon;
#endif

static constexpr Float Infinity = std::numeric_limits<Float>::infinity();
static constexpr Float MachineEpsilon = std::numeric_limits<Float>::epsilon() * 0.5;

#endif



template <typename T>
inline LUMIERE_HOST_DEVICE typename std::enable_if_t<std::is_floating_point<T>::value, bool> IsNaN(T v)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    return isnan(v);
#else
    return std::isnan(v);
#endif
}

template <typename T>
inline LUMIERE_HOST_DEVICE typename  std::enable_if_t<std::is_integral<T>::value, bool> IsNan(T v)
{
    return false;
}

template <typename T>
inline LUMIERE_HOST_DEVICE typename std::enable_if_t<std::is_floating_point<T>::value, bool> IsInf(T v)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    return isinf(v);
#else
    return std::isinf(v);
#endif
}

template <typename T>
inline LUMIERE_HOST_DEVICE typename std::enable_if_t<std::is_integral<T>::value, bool> IsInf(T v)
{
    return false;
}

LUMIERE_HOST_DEVICE
inline float FMA(float a, float b, float c)
{
    return std::fma(a, b, c);
}

LUMIERE_HOST_DEVICE
inline double FMA(double a, double b, double c)
{
    return std::fma(a, b, c);
}

inline long double FMA(long double a, long double b, long double c)
{
    return std::fma(a, b, c);
}

template <class To, class From>
LUMIERE_HOST_DEVICE typename std::enable_if_t<sizeof(To) == sizeof(From) &&
                                              std::is_trivially_copyable_v<From> &&
                                              std::is_trivially_copyable_v<To>, To>
bit_cast(const From &src) noexcept
{
    static_assert(std::is_trivially_constructible_v<To>,
                  "This implementation requires the destination type to be trivially "
                  "constructible");
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

LUMIERE_HOST_DEVICE
inline uint64_t FloatToBits(double f)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    return __double_as_longlong(f);
#else
    return bit_cast<uint64_t>(f);
#endif
}

LUMIERE_HOST_DEVICE
inline double BitsToFloat(uint64_t ui)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    return __longlong_as_double(ui);
#else
    return bit_cast<double>(ui);
#endif
}

LUMIERE_HOST_DEVICE
inline float NextFloatUp(float v)
{
    // Handle infinity and negative zero for _NextFloatUp()_
    if (IsInf(v) && v > 0.f)
        return v;
    if (v == -0.f)
        v = 0.f;

    // Advance _v_ to next higher float
    uint32_t ui = FloatToBits(v);
    if (v >= 0)
        ++ui;
    else
        --ui;
    return BitsToFloat(ui);
}

LUMIERE_HOST_DEVICE
inline float NextFloatDown(float v)
{
    // Handle infinity and positive zero for _NextFloatDown()_
    if (IsInf(v) && v < 0.)
        return v;
    if (v == 0.f)
        v = -0.f;
    uint32_t ui = FloatToBits(v);
    if (v > 0)
        --ui;
    else
        ++ui;
    return BitsToFloat(ui);
}

inline LUMIERE_HOST_DEVICE Float AddRoundUp(Float a, Float b)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __dadd_ru(a, b);
    #else
        return __fadd_ru(a, b);
    #endif
#else  // GPU
    return NextFloatUp(a + b);
#endif
}

inline LUMIERE_HOST_DEVICE Float AddRoundDown(Float a, Float b)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __dadd_rd(a, b);
    #else
        return __fadd_rd(a, b);
    #endif
#else  // GPU
    return NextFloatDown(a + b);
#endif
}

inline LUMIERE_HOST_DEVICE Float SubRoundUp(Float a, Float b)
{
    return AddRoundUp(a, -b);
}

inline LUMIERE_HOST_DEVICE Float SubRoundDown(Float a, Float b)
{
    return AddRoundDown(a, -b);
}

inline LUMIERE_HOST_DEVICE Float MulRoundUp(Float a, Float b)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __dmul_ru(a, b);
    #else
        return __fmul_ru(a, b);
    #endif
#else
    return NextFloatUp(a * b);
#endif
}

inline LUMIERE_HOST_DEVICE Float MulRoundDown(Float a, Float b)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __dmul_rd(a, b);
    #else
        return __fmul_rd(a, b);
    #endif
#else
    return NextFloatDown(a * b);
#endif
}

inline LUMIERE_HOST_DEVICE Float DivRoundUp(Float a, Float b)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __ddiv_ru(a, b);
    #else
        return __fdiv_ru(a, b);
    #endif
#else
    return NextFloatUp(a / b);
#endif
}

inline LUMIERE_HOST_DEVICE Float DivRoundDown(Float a, Float b)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __ddiv_rd(a, b);
    #else
        return __fdiv_rd(a, b);
    #endif
#else
    return NextFloatDown(a / b);
#endif
}

inline LUMIERE_HOST_DEVICE Float SqrtRoundUp(Float a)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __dsqrt_ru(a);
    #else
        return __fsqrt_ru(a);
    #endif
#else
    return NextFloatUp(std::sqrt(a));
#endif
}

inline LUMIERE_HOST_DEVICE Float SqrtRoundDown(Float a)
{
#ifdef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        return __dsqrt_rd(a);
    #else
        return __fsqrt_rd(a);
    #endif
#else
    return std::max<Float>(0, NextFloatDown(std::sqrt(a)));
#endif
}

END_LUMIERE_NAMESPACE