#pragma once
#include "Common/LumiereMacro.h"
#include "LumiereFloatDefs.h"


BEGIN_LUMIERE_NAMESPACE

#ifdef LUMIERE_IS_DEVICE_CODE

#define ShadowEpsilon 0.0001f
#define Pi Float(3.14159265358979323846)
#define InvPi Float(0.31830988618379067154)
#define Inv2Pi Float(0.15915494309189533577)
#define Inv4Pi Float(0.07957747154594766788)
#define PiOver2 Float(1.57079632679489661923)
#define PiOver4 Float(0.78539816339744830961)
#define Sqrt2 Float(1.41421356237309504880)

#else

// Mathematical Constants
constexpr Float ShadowEpsilon = 0.0001f;

constexpr Float Pi = 3.14159265358979323846;
constexpr Float InvPi = 0.31830988618379067154;
constexpr Float Inv2Pi = 0.15915494309189533577;
constexpr Float Inv4Pi = 0.07957747154594766788;
constexpr Float PiOver2 = 1.57079632679489661923;
constexpr Float PiOver4 = 0.78539816339744830961;
constexpr Float Sqrt2 = 1.41421356237309504880;

#endif


template <typename T>
inline LUMIERE_HOST_DEVICE typename std::enable_if_t<std::is_integral<T>::value, T> FMA(T a, T b, T c)
{
    return a * b + c;
}


template <typename Ta, typename Tb, typename Tc, typename Td>
LUMIERE_HOST_DEVICE inline auto DifferenceOfProducts(Ta a, Tb b, Tc c, Td d)
{
    auto cd = c * d;
    auto differenceOfProducts = FMA(a, b, -cd);
    auto error = FMA(-c, d, cd);
    return differenceOfProducts + error;
}

template <typename Ta, typename Tb, typename Tc, typename Td>
LUMIERE_HOST_DEVICE inline auto SumOfProducts(Ta a, Tb b, Tc c, Td d)
{
    auto cd = c * d;
    auto sumOfProducts = FMA(a, b, cd);
    auto error = FMA(c, d, -cd);
    return sumOfProducts + error;
}

template <typename T>
LUMIERE_HOST_DEVICE inline constexpr T Sqr(T v)
{
    return v * v;
}

END_LUMIERE_NAMESPACE