#include "LumiereVector.h"

BEGIN_LUMIERE_NAMESPACE

template <typename T>
std::string internal::ToString2(T x, T y)
{
    if (std::is_floating_point<T>::value)
        return StringPrintf("[ {}, {} ]", x, y);
    else
        return StringPrintf("[ {}, {} ]", x, y);
}

template <typename T>
std::string internal::ToString3(T x, T y, T z)
{
    if (std::is_floating_point<T>::value)
        return LUMIERE_FORMAT("[ {}, {}, {} ]", x, y, z);
    else
        return StringPrintf("[ {}, {}, {} ]", x, y, z);
}


#ifndef LUMIERE_IS_DEVICE_CODE
    #ifdef LUMIERE_FLOAT_AS_DOUBLE
        const Interval Interval::Pi(3.1415926535897931, 3.1415926535897936);
    #else
        const Interval Interval::Pi = Interval(3.1415925f, 3.14159274f);
    #endif
#endif


std::string Interval::ToString() const
{
    return LUMIERE_FORMAT("[ Interval [%f, %f] ]", low, high);
}

END_LUMIERE_NAMESPACE
