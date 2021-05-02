#pragma once

#include <algorithm>
#include <string>
#include "Common/LumiereMacro.h"
#include "LumiereMathStd.h"
#include "LumiereCheck.h"
#include "LumiereFloatDefs.h"

BEGIN_LUMIERE_NAMESPACE

class Interval;
template <typename T> class Point2;
template <typename T> class Point3;
template <typename T> class Normal3;

namespace internal {

template <typename T> std::string ToString2(T x, T y);
template <typename T> std::string ToString3(T x, T y, T z);

} // namespace internal


namespace {

template <typename T>
struct TupleLength {
    using type = Float;
};

template <>
struct TupleLength<double> {
    using type = double;
};

template <>
struct TupleLength<long double> {
    using type = long double;
};

template <>
struct TupleLength<Interval> {
    using type = Interval;
};

}


class Interval {
public:
    Interval() = default;
    LUMIERE_HOST_DEVICE explicit Interval(Float v) : low(v), high(v) {}
    LUMIERE_HOST_DEVICE constexpr Interval(Float low, Float high): low(std::min(low, high)), high(std::max(low, high)) {}

public:
    LUMIERE_HOST_DEVICE
    static Interval FromValueAndError(Float v, Float err) {
        Interval i{};
        if (err == 0)
            i.low = i.high = v;
        else {
            i.low = SubRoundDown(v, err);
            i.high = AddRoundUp(v, err);
        }
        return i;
    }

    LUMIERE_HOST_DEVICE
    Interval &operator=(Float v) {
        low = high = v;
        return *this;
    }

    LUMIERE_HOST_DEVICE Float UpperBound() const { return high; }
    LUMIERE_HOST_DEVICE Float LowerBound() const { return low; }
    LUMIERE_HOST_DEVICE Float Midpoint() const { return (low + high) / 2; }
    LUMIERE_HOST_DEVICE Float Width() const { return high - low; }

    LUMIERE_HOST_DEVICE
    Float operator[](int i) const
    {
        DCHECK(i == 0 || i == 1);
        return (i == 0) ? low : high;
    }

    LUMIERE_HOST_DEVICE
    explicit operator Float() const { return Midpoint(); }

    LUMIERE_HOST_DEVICE
    bool Exactly(Float v) const { return low == v && high == v; }

    LUMIERE_HOST_DEVICE
    bool operator==(Float v) const { return Exactly(v); }

    LUMIERE_HOST_DEVICE
    Interval operator-() const { return {-high, -low}; }

    LUMIERE_HOST_DEVICE
    Interval operator+(Interval i) const
    {
        return {AddRoundDown(low, i.low), AddRoundUp(high, i.high)};
    }

    LUMIERE_HOST_DEVICE
    Interval operator-(Interval i) const
    {
        return {SubRoundDown(low, i.high), SubRoundUp(high, i.low)};
    }

    LUMIERE_HOST_DEVICE
    Interval operator*(Interval i) const
    {
        Float lp[4] = {MulRoundDown(low, i.low), MulRoundDown(high, i.low),
                       MulRoundDown(low, i.high), MulRoundDown(high, i.high)};
        Float hp[4] = {MulRoundUp(low, i.low), MulRoundUp(high, i.low),
                       MulRoundUp(low, i.high), MulRoundUp(high, i.high)};
        return {std::min({lp[0], lp[1], lp[2], lp[3]}),
                std::max({hp[0], hp[1], hp[2], hp[3]})};
    }

    LUMIERE_HOST_DEVICE Interval operator/(Interval i) const;

    LUMIERE_HOST_DEVICE
    bool operator==(Interval i) const
    {
        return low == i.low && high == i.high;
    }

    LUMIERE_HOST_DEVICE
    bool operator!=(Float f) const { return f < low || f > high; }

    std::string ToString() const;

    LUMIERE_HOST_DEVICE Interval &operator+=(Interval i)
    {
        *this = Interval(*this + i);
        return *this;
    }

    LUMIERE_HOST_DEVICE Interval &operator-=(Interval i)
    {
        *this = Interval(*this - i);
        return *this;
    }

    LUMIERE_HOST_DEVICE Interval &operator*=(Interval i)
    {
        *this = Interval(*this * i);
        return *this;
    }

    LUMIERE_HOST_DEVICE Interval &operator/=(Interval i)
    {
        *this = Interval(*this / i);
        return *this;
    }

    LUMIERE_HOST_DEVICE Interval &operator+=(Float f) { return *this += Interval(f); }
    LUMIERE_HOST_DEVICE Interval &operator-=(Float f) { return *this -= Interval(f); }
    LUMIERE_HOST_DEVICE Interval &operator*=(Float f) { return *this *= Interval(f); }
    LUMIERE_HOST_DEVICE Interval &operator/=(Float f) { return *this /= Interval(f); }

#ifndef LUMIERE_IS_DEVICE_CODE
    static const Interval Pi;
#endif

private:
    Float low, high;
};




template <template <typename> class Child, typename T>
class Tuple2 {
public:
    static const int nDimensions = 2;

public:
    Tuple2() = default;

    LUMIERE_HOST_DEVICE Tuple2(T x, T y) : x(x), y(y) { DCHECK(!HasNaN()); }
    LUMIERE_HOST_DEVICE bool HasNaN() const { return IsNaN(x) || IsNaN(y); }

#ifdef LUMIERE_ENABLE_DEBUG
    LUMIERE_HOST_DEVICE
    Tuple2(const Child<T> &c) {
        DCHECK(!c.HasNaN());
        x = c.x;
        y = c.y;
    }

    LUMIERE_HOST_DEVICE
    Child<T> &operator=(const Child<T> &c) {
        DCHECK(!c.HasNaN());
        x = c.x;
        y = c.y;
        return static_cast<Child<T> &>(*this);
    }
#endif

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator+(const Child<U> &c) const -> Child<decltype(T{} + U{})> {
        DCHECK(!c.HasNaN());
        return {x + c.x, y + c.y};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator+=(const Child<U> &c) {
        DCHECK(!c.HasNaN());
        x += c.x;
        y += c.y;
        return static_cast<Child<T> &>(*this);
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator-(const Child<U> &c) const -> Child<decltype(T{} - U{})> {
        DCHECK(!c.HasNaN());
        return {x - c.x, y - c.y};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator-=(const Child<U> &c) {
        DCHECK(!c.HasNaN());
        x -= c.x;
        y -= c.y;
        return static_cast<Child<T> &>(*this);
    }

    LUMIERE_HOST_DEVICE bool operator==(const Child<T> &c) const { return x == c.x && y == c.y; }
    LUMIERE_HOST_DEVICE bool operator!=(const Child<T> &c) const { return x != c.x || y != c.y; }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator*(U s) const -> Child<decltype(T{} * U{})>
    {
        return {s * x, s * y};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator*=(U s)
    {
        DCHECK(!IsNaN(s));
        x *= s;
        y *= s;
        return static_cast<Child<T> &>(*this);
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator/(U d) const -> Child<decltype(T{} / U{})>
    {
        DCHECK(d != 0 && !IsNaN(d));
        return {x / d, y / d};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator/=(U d)
    {
        DCHECK_NE(d, 0);
        DCHECK(!IsNaN(d));
        x /= d;
        y /= d;
        return static_cast<Child<T> &>(*this);
    }

    LUMIERE_HOST_DEVICE Child<T> operator-() const { return {-x, -y}; }

    LUMIERE_HOST_DEVICE
    T operator[](int i) const
    {
        DCHECK(i >= 0 && i <= 1);
        return (i == 0) ? x : y;
    }

    LUMIERE_HOST_DEVICE
    T &operator[](int i)
    {
        DCHECK(i >= 0 && i <= 1);
        return (i == 0) ? x : y;
    }

    std::string ToString() const { return internal::ToString2(x, y); }

public:
    T x = {};
    T y = {};
};


template <template <class> class C, typename T, typename U>
LUMIERE_HOST_DEVICE inline auto operator*(U s, const Tuple2<C, T> &t) -> C<decltype(T{} * U{})>
{
    DCHECK(!t.HasNaN());
    return t * s;
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Abs(const Tuple2<C, T> &t)
{
    // "argument-dependent lookup..." (here and elsewhere)
    using std::abs;
    return {abs(t.x), abs(t.y)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Ceil(const Tuple2<C, T> &t)
{
    using std::ceil;
    return {ceil(t.x), ceil(t.y)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Floor(const Tuple2<C, T> &t)
{
    using std::floor;
    return {floor(t.x), floor(t.y)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline auto Lerp(Float t, const Tuple2<C, T> &t0, const Tuple2<C, T> &t1)
{
    return (1 - t) * t0 + t * t1;
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> FMA(Float a, const Tuple2<C, T> &b, const Tuple2<C, T> &c)
{
    return {FMA(a, b.x, c.x), FMA(a, b.y, c.y)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> FMA(const Tuple2<C, T> &a, Float b, const Tuple2<C, T> &c)
{
    return FMA(b, a, c);
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Min(const Tuple2<C, T> &t0, const Tuple2<C, T> &t1)
{
    using std::min;
    return {min(t0.x, t1.x), min(t0.y, t1.y)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline T MinComponentValue(const Tuple2<C, T> &t)
{
    using std::min;
    return min({t.x, t.y});
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline int MinComponentIndex(const Tuple2<C, T> &t)
{
    return (t.x < t.y) ? 0 : 1;
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Max(const Tuple2<C, T> &t0, const Tuple2<C, T> &t1)
{
    using std::max;
    return {max(t0.x, t1.x), max(t0.y, t1.y)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline T MaxComponentValue(const Tuple2<C, T> &t)
{
    using std::max;
    return max({t.x, t.y});
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline int MaxComponentIndex(const Tuple2<C, T> &t)
{
    return (t.x > t.y) ? 0 : 1;
}

//template <template <class> class C, typename T>
//LUMIERE_HOST_DEVICE inline C<T> Permute(const Tuple2<C, T> &t, pstd::array<int, 2> p)
//{
//    return {t[p[0]], t[p[1]]};
//}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline T HProd(const Tuple2<C, T> &t)
{
    return t.x * t.y;
}




template <template <typename> class Child, typename T>
class Tuple3 {
public:
    Tuple3() = default;
    LUMIERE_HOST_DEVICE Tuple3(T x, T y, T z) : x(x), y(y), z(z) { DCHECK(!HasNaN()); }

public:
    LUMIERE_HOST_DEVICE bool HasNaN() const { return IsNaN(x) || IsNaN(y) || IsNaN(z); }

    LUMIERE_HOST_DEVICE
    T operator[](int i) const
    {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }

    LUMIERE_HOST_DEVICE
    T &operator[](int i)
    {
        DCHECK(i >= 0 && i <= 2);
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator+(const Child<U> &c) const -> Child<decltype(T{} + U{})>
    {
        DCHECK(!c.HasNaN());
        return {x + c.x, y + c.y, z + c.z};
    }

    static const int nDimensions = 3;

#ifdef LUMIERE_ENABLE_DEBUG
    LUMIERE_HOST_DEVICE
    Tuple3(const Child<T> &c)
    {
        DCHECK(!c.HasNaN());
        x = c.x;
        y = c.y;
        z = c.z;
    }

    LUMIERE_HOST_DEVICE
    Child<T> &operator=(const Child<T> &c)
    {
        DCHECK(!c.HasNaN());
        x = c.x;
        y = c.y;
        z = c.z;
        return static_cast<Child<T> &>(*this);
    }
#endif

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator+=(const Child<U> &c)
    {
        DCHECK(!c.HasNaN());
        x += c.x;
        y += c.y;
        z += c.z;
        return static_cast<Child<T> &>(*this);
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator-(const Child<U> &c) const -> Child<decltype(T{} - U{})>
    {
        DCHECK(!c.HasNaN());
        return {x - c.x, y - c.y, z - c.z};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator-=(const Child<U> &c)
    {
        DCHECK(!c.HasNaN());
        x -= c.x;
        y -= c.y;
        z -= c.z;
        return static_cast<Child<T> &>(*this);
    }

    LUMIERE_HOST_DEVICE bool operator==(const Child<T> &c) const { return x == c.x && y == c.y && z == c.z; }
    LUMIERE_HOST_DEVICE bool operator!=(const Child<T> &c) const { return x != c.x || y != c.y || z != c.z; }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator*(U s) const -> Child<decltype(T{} * U{})>
    {
        return {s * x, s * y, s * z};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator*=(U s)
    {
        DCHECK(!IsNaN(s));
        x *= s;
        y *= s;
        z *= s;
        return static_cast<Child<T> &>(*this);
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator/(U d) const -> Child<decltype(T{} / U{})>
    {
        DCHECK_NE(d, 0);
        return {x / d, y / d, z / d};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Child<T> &operator/=(U d)
    {
        DCHECK_NE(d, 0);
        x /= d;
        y /= d;
        z /= d;
        return static_cast<Child<T> &>(*this);
    }

    LUMIERE_HOST_DEVICE
    Child<T> operator-() const { return {-x, -y, -z}; }

    std::string ToString() const { return internal::ToString3(x, y, z); }

public:
    T x{}, y{}, z{};
};


template <template <class> class C, typename T, typename U>
LUMIERE_HOST_DEVICE inline auto operator*(U s, const Tuple3<C, T> &t) -> C<decltype(T{} * U{})>
{
    return t * s;
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Abs(const Tuple3<C, T> &t)
{
    using std::abs;
    return {abs(t.x), abs(t.y), abs(t.z)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Ceil(const Tuple3<C, T> &t)
{
    using std::ceil;
    return {ceil(t.x), ceil(t.y), ceil(t.z)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Floor(const Tuple3<C, T> &t)
{
    using std::floor;
    return {floor(t.x), floor(t.y), floor(t.z)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline auto Lerp(Float t, const Tuple3<C, T> &t0, const Tuple3<C, T> &t1)
{
    return (1 - t) * t0 + t * t1;
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> FMA(Float a, const Tuple3<C, T> &b, const Tuple3<C, T> &c)
{
    return {FMA(a, b.x, c.x), FMA(a, b.y, c.y), FMA(a, b.z, c.z)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> FMA(const Tuple3<C, T> &a, Float b, const Tuple3<C, T> &c)
{
    return FMA(b, a, c);
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Min(const Tuple3<C, T> &t1, const Tuple3<C, T> &t2)
{
    using std::min;
    return {min(t1.x, t2.x), min(t1.y, t2.y), min(t1.z, t2.z)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline T MinComponentValue(const Tuple3<C, T> &t)
{
    using std::min;
    return min({t.x, t.y, t.z});
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline int MinComponentIndex(const Tuple3<C, T> &t)
{
    return (t.x < t.y) ? ((t.x < t.z) ? 0 : 2) : ((t.y < t.z) ? 1 : 2);
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline C<T> Max(const Tuple3<C, T> &t1, const Tuple3<C, T> &t2)
{
    using std::max;
    return {max(t1.x, t2.x), max(t1.y, t2.y), max(t1.z, t2.z)};
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline T MaxComponentValue(const Tuple3<C, T> &t)
{
    using std::max;
    return max({t.x, t.y, t.z});
}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline int MaxComponentIndex(const Tuple3<C, T> &t)
{
    return (t.x > t.y) ? ((t.x > t.z) ? 0 : 2) : ((t.y > t.z) ? 1 : 2);
}

//template <template <class> class C, typename T>
//LUMIERE_HOST_DEVICE inline C<T> Permute(const Tuple3<C, T> &t, pstd::array<int, 3> p)
//{
//    return {t[p[0]], t[p[1]], t[p[2]]};
//}

template <template <class> class C, typename T>
LUMIERE_HOST_DEVICE inline T HProd(const Tuple3<C, T> &t)
{
    return t.x * t.y * t.z;
}




template <typename T>
class Vector2 : public Tuple2<Vector2, T> {
public:
    using Tuple2<Vector2, T>::x;
    using Tuple2<Vector2, T>::y;

    Vector2() = default;

    LUMIERE_HOST_DEVICE
    Vector2(T x, T y) : Tuple2<Vector2, T>(x, y) {}
    template <typename U>
    LUMIERE_HOST_DEVICE explicit Vector2(const Point2<U> &p);
    template <typename U>
    LUMIERE_HOST_DEVICE explicit Vector2(const Vector2<U> &v): Tuple2<Vector2, T>(T(v.x), T(v.y)) {}
};


template <typename T>
class Vector3 : public Tuple3<Vector3, T> {
public:
    using Tuple3<Vector3, T>::x;
    using Tuple3<Vector3, T>::y;
    using Tuple3<Vector3, T>::z;

public:
    Vector3() = default;
    LUMIERE_HOST_DEVICE Vector3(T x, T y, T z) : Tuple3<Vector3, T>(x, y, z) {}

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Vector3(const Vector3<U> &v) : Tuple3<Vector3, T>(T(v.x), T(v.y), T(v.z)) {}

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Vector3(const Point3<U> &p);

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Vector3(const Normal3<U> &n);
};


using Vector2f = Vector2<Float>;
using Vector2i = Vector2<int>;
using Vector3f = Vector3<Float>;
using Vector3i = Vector3<int>;




template <typename T>
class Point2 : public Tuple2<Point2, T> {
public:
    using Tuple2<Point2, T>::x;
    using Tuple2<Point2, T>::y;
    using Tuple2<Point2, T>::HasNaN;
    using Tuple2<Point2, T>::operator+;
    using Tuple2<Point2, T>::operator+=;
    using Tuple2<Point2, T>::operator*;
    using Tuple2<Point2, T>::operator*=;

public:
    LUMIERE_HOST_DEVICE Point2() { x = y = 0; }
    LUMIERE_HOST_DEVICE Point2(T x, T y) : Tuple2<Point2, T>(x, y) {}

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Point2(const Point2<U> &v): Tuple2<Point2, T>(T(v.x), T(v.y)) {}
    template <typename U>
    LUMIERE_HOST_DEVICE explicit Point2(const Vector2<U> &v): Tuple2<Point2, T>(T(v.x), T(v.y)) {}

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator+(const Vector2<U> &v) const -> Point2<decltype(T{} + U{})>
    {
        DCHECK(!v.HasNaN());
        return {x + v.x, y + v.y};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Point2<T> &operator+=(const Vector2<U> &v)
    {
        DCHECK(!v.HasNaN());
        x += v.x;
        y += v.y;
        return *this;
    }

    LUMIERE_HOST_DEVICE Point2<T> operator-() const { return {-x, -y}; }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator-(const Point2<U> &p) const -> Vector2<decltype(T{} - U{})>
    {
        DCHECK(!p.HasNaN());
        return {x - p.x, y - p.y};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator-(const Vector2<U> &v) const -> Point2<decltype(T{} - U{})>
    {
        DCHECK(!v.HasNaN());
        return {x - v.x, y - v.y};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Point2<T> &operator-=(const Vector2<U> &v)
    {
        DCHECK(!v.HasNaN());
        x -= v.x;
        y -= v.y;
        return *this;
    }
};


template <typename T>
class Point3 : public Tuple3<Point3, T> {
public:
    using Tuple3<Point3, T>::x;
    using Tuple3<Point3, T>::y;
    using Tuple3<Point3, T>::z;
    using Tuple3<Point3, T>::HasNaN;
    using Tuple3<Point3, T>::operator+;
    using Tuple3<Point3, T>::operator+=;
    using Tuple3<Point3, T>::operator*;
    using Tuple3<Point3, T>::operator*=;

public:
    Point3() = default;
    LUMIERE_HOST_DEVICE
    Point3(T x, T y, T z) : Tuple3<Point3, T>(x, y, z) {}

    LUMIERE_HOST_DEVICE
    Point3<T> operator-() const { return {-x, -y, -z}; }

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Point3(const Point3<U> &p): Tuple3<Point3, T>(T(p.x), T(p.y), T(p.z)) {}
    template <typename U>
    LUMIERE_HOST_DEVICE explicit Point3(const Vector3<U> &v): Tuple3<Point3, T>(T(v.x), T(v.y), T(v.z)) {}

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator+(const Vector3<U> &v) const -> Point3<decltype(T{} + U{})>
    {
        DCHECK(!v.HasNaN());
        return {x + v.x, y + v.y, z + v.z};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Point3<T> &operator+=(const Vector3<U> &v)
    {
        DCHECK(!v.HasNaN());
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator-(const Vector3<U> &v) const -> Point3<decltype(T{} - U{})>
    {
        DCHECK(!v.HasNaN());
        return {x - v.x, y - v.y, z - v.z};
    }

    template <typename U>
    LUMIERE_HOST_DEVICE Point3<T> &operator-=(const Vector3<U> &v)
    {
        DCHECK(!v.HasNaN());
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    template <typename U>
    LUMIERE_HOST_DEVICE auto operator-(const Point3<U> &p) const -> Vector3<decltype(T{} - U{})>
    {
        DCHECK(!p.HasNaN());
        return {x - p.x, y - p.y, z - p.z};
    }
};


using Point2f = Point2<Float>;
using Point2i = Point2<int>;
using Point3f = Point3<Float>;
using Point3i = Point3<int>;


template <typename T>
class Normal3 : public Tuple3<Normal3, T> {
public:
    using Tuple3<Normal3, T>::x;
    using Tuple3<Normal3, T>::y;
    using Tuple3<Normal3, T>::z;
    using Tuple3<Normal3, T>::HasNaN;
    using Tuple3<Normal3, T>::operator+;
    using Tuple3<Normal3, T>::operator*;
    using Tuple3<Normal3, T>::operator*=;

public:
    Normal3() = default;
    LUMIERE_HOST_DEVICE Normal3(T x, T y, T z): Tuple3<Normal3, T>(x, y, z) {}

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Normal3<T>(const Normal3<U> &v): Tuple3<Normal3, T>(T(v.x), T(v.y), T(v.z)) {}

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Normal3<T>(const Vector3<U> &v): Tuple3<Normal3, T>(T(v.x), T(v.y), T(v.z)) {}
};

using Normal3f = Normal3<Float>;



template <typename T>
template <typename U>
Vector2<T>::Vector2(const Point2<U> &p) : Tuple2<Vector2, T>(T(p.x), T(p.y)) {}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Dot(const Vector2<T> &v1, const Vector2<T> &v2) -> typename TupleLength<T>::type
{
    DCHECK(!v1.HasNaN() && !v2.HasNaN());
    return SumOfProducts(v1.x, v2.x, v1.y, v2.y);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto AbsDot(const Vector2<T> &v1, const Vector2<T> &v2) -> typename TupleLength<T>::type
{
    DCHECK(!v1.HasNaN() && !v2.HasNaN());
    return std::abs(Dot(v1, v2));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto LengthSquared(const Vector2<T> &v) -> typename TupleLength<T>::type
{
    return Sqr(v.x) + Sqr(v.y);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Length(const Vector2<T> &v) -> typename TupleLength<T>::type
{
    using std::sqrt;
    return sqrt(LengthSquared(v));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Normalize(const Vector2<T> &v)
{
    return v / Length(v);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Distance(const Point2<T> &p1, const Point2<T> &p2) -> typename TupleLength<T>::type
{
    return Length(p1 - p2);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto DistanceSquared(const Point2<T> &p1, const Point2<T> &p2) -> typename TupleLength<T>::type
{
    return LengthSquared(p1 - p2);
}

template <typename T>
template <typename U>
Vector3<T>::Vector3(const Point3<U> &p) : Tuple3<Vector3, T>(T(p.x), T(p.y), T(p.z)) {}

template <typename T>
LUMIERE_HOST_DEVICE inline Vector3<T> Cross(const Vector3<T> &v1, const Normal3<T> &v2)
{
    DCHECK(!v1.HasNaN() && !v2.HasNaN());
    return {DifferenceOfProducts(v1.y, v2.z, v1.z, v2.y),
            DifferenceOfProducts(v1.z, v2.x, v1.x, v2.z),
            DifferenceOfProducts(v1.x, v2.y, v1.y, v2.x)};
}

template <typename T>
LUMIERE_HOST_DEVICE inline Vector3<T> Cross(const Normal3<T> &v1, const Vector3<T> &v2)
{
    DCHECK(!v1.HasNaN() && !v2.HasNaN());
    return {DifferenceOfProducts(v1.y, v2.z, v1.z, v2.y),
            DifferenceOfProducts(v1.z, v2.x, v1.x, v2.z),
            DifferenceOfProducts(v1.x, v2.y, v1.y, v2.x)};
}

template <typename T>
LUMIERE_HOST_DEVICE inline T LengthSquared(const Vector3<T> &v)
{
    return Sqr(v.x) + Sqr(v.y) + Sqr(v.z);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Length(const Vector3<T> &v) -> typename TupleLength<T>::type
{
    using std::sqrt;
    return sqrt(LengthSquared(v));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Normalize(const Vector3<T> &v)
{
    return v / Length(v);
}

template <typename T>
LUMIERE_HOST_DEVICE inline T Dot(const Vector3<T> &v, const Vector3<T> &w)
{
    DCHECK(!v.HasNaN() && !w.HasNaN());
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

// Equivalent to std::acos(Dot(a, b)), but more numerically stable.
// via http://www.plunk.org/~hatch/rightway.php
template <typename T>
LUMIERE_HOST_DEVICE inline Float AngleBetween(const Vector3<T> &v1, const Vector3<T> &v2)
{
    if (Dot(v1, v2) < 0)
        return Pi - 2 * SafeASin(Length(v1 + v2) / 2);
    else
        return 2 * SafeASin(Length(v2 - v1) / 2);
}

template <typename T>
LUMIERE_HOST_DEVICE inline T AbsDot(const Vector3<T> &v1, const Vector3<T> &v2)
{
    DCHECK(!v1.HasNaN() && !v2.HasNaN());
    return std::abs(Dot(v1, v2));
}

template <typename T>
LUMIERE_HOST_DEVICE inline Float AngleBetween(const Normal3<T> &a, const Normal3<T> &b)
{
    if (Dot(a, b) < 0)
        return Pi - 2 * SafeASin(Length(a + b) / 2);
    else
        return 2 * SafeASin(Length(b - a) / 2);
}

template <typename T>
LUMIERE_HOST_DEVICE inline Vector3<T> GramSchmidt(const Vector3<T> &v, const Vector3<T> &w)
{
    return v - Dot(v, w) * w;
}

template <typename T>
LUMIERE_HOST_DEVICE inline Vector3<T> Cross(const Vector3<T> &v, const Vector3<T> &w)
{
    DCHECK(!v.HasNaN() && !w.HasNaN());
    return {DifferenceOfProducts(v.y, w.z, v.z, w.y),
            DifferenceOfProducts(v.z, w.x, v.x, w.z),
            DifferenceOfProducts(v.x, w.y, v.y, w.x)};
}

template <typename T>
LUMIERE_HOST_DEVICE inline void CoordinateSystem(const Vector3<T> &v1, Vector3<T> *v2, Vector3<T> *v3)
{
    Float sign = std::copysign(Float(1), v1.z);
    Float a = -1 / (sign + v1.z);
    Float b = v1.x * v1.y * a;
    *v2 = Vector3<T>(1 + sign * v1.x * v1.x * a, sign * b, -sign * v1.x);
    *v3 = Vector3<T>(b, sign + v1.y * v1.y * a, -v1.y);
}

template <typename T>
LUMIERE_HOST_DEVICE inline void CoordinateSystem(const Normal3<T> &v1, Vector3<T> *v2, Vector3<T> *v3)
{
    Float sign = std::copysign(Float(1), v1.z);
    Float a = -1 / (sign + v1.z);
    Float b = v1.x * v1.y * a;
    *v2 = Vector3<T>(1 + sign * v1.x * v1.x * a, sign * b, -sign * v1.x);
    *v3 = Vector3<T>(b, sign + v1.y * v1.y * a, -v1.y);
}

template <typename T>
template <typename U>
Vector3<T>::Vector3(const Normal3<U> &n) : Tuple3<Vector3, T>(T(n.x), T(n.y), T(n.z)) {}


template <typename T>
LUMIERE_HOST_DEVICE inline auto Distance(const Point3<T> &p1, const Point3<T> &p2)
{
    return Length(p1 - p2);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto DistanceSquared(const Point3<T> &p1, const Point3<T> &p2)
{
    return LengthSquared(p1 - p2);
}


template <typename T>
LUMIERE_HOST_DEVICE inline auto LengthSquared(const Normal3<T> &n) -> typename TupleLength<T>::type
{
    return Sqr(n.x) + Sqr(n.y) + Sqr(n.z);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Length(const Normal3<T> &n) -> typename TupleLength<T>::type
{
    using std::sqrt;
    return sqrt(LengthSquared(n));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Normalize(const Normal3<T> &n)
{
    return n / Length(n);
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Dot(const Normal3<T> &n, const Vector3<T> &v) -> typename TupleLength<T>::type
{
    DCHECK(!n.HasNaN() && !v.HasNaN());
    return FMA(n.x, v.x, SumOfProducts(n.y, v.y, n.z, v.z));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Dot(const Vector3<T> &v, const Normal3<T> &n) -> typename TupleLength<T>::type
{
    DCHECK(!v.HasNaN() && !n.HasNaN());
    return FMA(n.x, v.x, SumOfProducts(n.y, v.y, n.z, v.z));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto Dot(const Normal3<T> &n1, const Normal3<T> &n2) -> typename TupleLength<T>::type
{
    DCHECK(!n1.HasNaN() && !n2.HasNaN());
    return FMA(n1.x, n2.x, SumOfProducts(n1.y, n2.y, n1.z, n2.z));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto AbsDot(const Normal3<T> &n, const Vector3<T> &v) -> typename TupleLength<T>::type
{
    DCHECK(!n.HasNaN() && !v.HasNaN());
    return std::abs(Dot(n, v));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto AbsDot(const Vector3<T> &v, const Normal3<T> &n) -> typename TupleLength<T>::type
{
    using std::abs;
    DCHECK(!v.HasNaN() && !n.HasNaN());
    return abs(Dot(v, n));
}

template <typename T>
LUMIERE_HOST_DEVICE inline auto AbsDot(const Normal3<T> &n1, const Normal3<T> &n2) -> typename TupleLength<T>::type
{
    using std::abs;
    DCHECK(!n1.HasNaN() && !n2.HasNaN());
    return abs(Dot(n1, n2));
}

template <typename T>
LUMIERE_HOST_DEVICE inline Normal3<T> FaceForward(const Normal3<T> &n, const Vector3<T> &v)
{
    return (Dot(n, v) < 0.f) ? -n : n;
}

template <typename T>
LUMIERE_HOST_DEVICE inline Normal3<T> FaceForward(const Normal3<T> &n, const Normal3<T> &n2)
{
    return (Dot(n, n2) < 0.f) ? -n : n;
}

template <typename T>
LUMIERE_HOST_DEVICE inline Vector3<T> FaceForward(const Vector3<T> &v, const Vector3<T> &v2)
{
    return (Dot(v, v2) < 0.f) ? -v : v;
}

template <typename T>
LUMIERE_HOST_DEVICE inline Vector3<T> FaceForward(const Vector3<T> &v, const Normal3<T> &n2)
{
    return (Dot(v, n2) < 0.f) ? -v : v;
}




template <typename T>
class Bounds2 {
public:
    LUMIERE_HOST_DEVICE
    Bounds2()
    {
        T minNum = std::numeric_limits<T>::lowest();
        T maxNum = std::numeric_limits<T>::max();
        pMin = Point2<T>(maxNum, maxNum);
        pMax = Point2<T>(minNum, minNum);
    }

    LUMIERE_HOST_DEVICE explicit Bounds2(const Point2<T> &p) : pMin(p), pMax(p) {}
    LUMIERE_HOST_DEVICE Bounds2(const Point2<T> &p1, const Point2<T> &p2): pMin(Min(p1, p2)), pMax(Max(p1, p2)) {}

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Bounds2(const Bounds2<U> &b)
    {
        if (b.IsEmpty())
            // Be careful about overflowing float->int conversions and the
            // like.
            *this = Bounds2<T>();
        else {
            pMin = Point2<T>(b.pMin);
            pMax = Point2<T>(b.pMax);
        }
    }

    LUMIERE_HOST_DEVICE
    Vector2<T> Diagonal() const { return pMax - pMin; }

    LUMIERE_HOST_DEVICE
    T Area() const
    {
        Vector2<T> d = pMax - pMin;
        return d.x * d.y;
    }

    LUMIERE_HOST_DEVICE
    bool IsEmpty() const { return pMin.x >= pMax.x || pMin.y >= pMax.y; }

    LUMIERE_HOST_DEVICE
    bool IsDegenerate() const { return pMin.x > pMax.x || pMin.y > pMax.y; }

    LUMIERE_HOST_DEVICE
    int MaxDimension() const
    {
        Vector2<T> diag = Diagonal();
        if (diag.x > diag.y)
            return 0;
        else
            return 1;
    }

    LUMIERE_HOST_DEVICE
    const Point2<T> &operator[](int i) const
    {
        DCHECK(i == 0 || i == 1);
        return (i == 0) ? pMin : pMax;
    }

    LUMIERE_HOST_DEVICE
    Point2<T> &operator[](int i)
    {
        DCHECK(i == 0 || i == 1);
        return (i == 0) ? pMin : pMax;
    }

    LUMIERE_HOST_DEVICE
    bool operator==(const Bounds2<T> &b) const
    {
        return b.pMin == pMin && b.pMax == pMax;
    }

    LUMIERE_HOST_DEVICE
    bool operator!=(const Bounds2<T> &b) const
    {
        return b.pMin != pMin || b.pMax != pMax;
    }

    LUMIERE_HOST_DEVICE
    Point2<T> Corner(int corner) const
    {
        DCHECK(corner >= 0 && corner < 4);
        return Point2<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y);
    }

    LUMIERE_HOST_DEVICE
    Point2<T> Lerp(Point2f t) const
    {
        return Point2<T>(Lerp(t.x, pMin.x, pMax.x), Lerp(t.y, pMin.y, pMax.y));
    }

    LUMIERE_HOST_DEVICE
    Vector2<T> Offset(const Point2<T> &p) const
    {
        Vector2<T> o = p - pMin;
        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;
        return o;
    }

    LUMIERE_HOST_DEVICE
    void BoundingSphere(Point2<T> *c, Float *rad) const
    {
        *c = (pMin + pMax) / 2;
        *rad = Inside(*c, *this) ? Distance(*c, pMax) : 0;
    }

    std::string ToString() const { return LUMIERE_FORMAT("[ {} - {} ]", pMin, pMax); }

public:
    Point2<T> pMin, pMax;
};




template <typename T>
class Bounds3 {
public:
    LUMIERE_HOST_DEVICE
    Bounds3() {
        T minNum = std::numeric_limits<T>::lowest();
        T maxNum = std::numeric_limits<T>::max();
        pMin = Point3<T>(maxNum, maxNum, maxNum);
        pMax = Point3<T>(minNum, minNum, minNum);
    }

    LUMIERE_HOST_DEVICE
    explicit Bounds3(const Point3<T> &p) : pMin(p), pMax(p) {}

    LUMIERE_HOST_DEVICE
    Bounds3(const Point3<T> &p1, const Point3<T> &p2)
            : pMin(Min(p1, p2)), pMax(Max(p1, p2)) {}

    LUMIERE_HOST_DEVICE
    const Point3<T> &operator[](int i) const
    {
        DCHECK(i == 0 || i == 1);
        return (i == 0) ? pMin : pMax;
    }

    LUMIERE_HOST_DEVICE Point3<T> &operator[](int i)
    {
        DCHECK(i == 0 || i == 1);
        return (i == 0) ? pMin : pMax;
    }

    LUMIERE_HOST_DEVICE
    Point3<T> Corner(int corner) const
    {
        DCHECK(corner >= 0 && corner < 8);
        return Point3<T>((*this)[(corner & 1)].x, (*this)[(corner & 2) ? 1 : 0].y,
                         (*this)[(corner & 4) ? 1 : 0].z);
    }

    LUMIERE_HOST_DEVICE Vector3<T> Diagonal() const { return pMax - pMin; }

    LUMIERE_HOST_DEVICE
    T SurfaceArea() const
    {
        Vector3<T> d = Diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    LUMIERE_HOST_DEVICE T Volume() const
    {
        Vector3<T> d = Diagonal();
        return d.x * d.y * d.z;
    }

    LUMIERE_HOST_DEVICE
    int MaxDimension() const
    {
        Vector3<T> d = Diagonal();
        if (d.x > d.y && d.x > d.z)
            return 0;
        else if (d.y > d.z)
            return 1;
        else
            return 2;
    }

    LUMIERE_HOST_DEVICE
    Point3<T> Lerp(Point3f t) const
    {
        return Point3<T>(Lerp(t.x, pMin.x, pMax.x), Lerp(t.y, pMin.y, pMax.y), Lerp(t.z, pMin.z, pMax.z));
    }

    LUMIERE_HOST_DEVICE
    Vector3<T> Offset(const Point3<T> &p) const
    {
        Vector3<T> o = p - pMin;
        if (pMax.x > pMin.x)
            o.x /= pMax.x - pMin.x;
        if (pMax.y > pMin.y)
            o.y /= pMax.y - pMin.y;
        if (pMax.z > pMin.z)
            o.z /= pMax.z - pMin.z;
        return o;
    }

    LUMIERE_HOST_DEVICE
    void BoundingSphere(Point3<T> *center, Float *radius) const
    {
        *center = (pMin + pMax) / 2;
        *radius = Inside(*center, *this) ? Distance(*center, pMax) : 0;
    }

    LUMIERE_HOST_DEVICE
    bool IsEmpty() const
    {
        return pMin.x >= pMax.x || pMin.y >= pMax.y || pMin.z >= pMax.z;
    }

    LUMIERE_HOST_DEVICE
    bool IsDegenerate() const
    {
        return pMin.x > pMax.x || pMin.y > pMax.y || pMin.z > pMax.z;
    }

    template <typename U>
    LUMIERE_HOST_DEVICE explicit Bounds3(const Bounds3<U> &b)
    {
        if (b.IsEmpty())
            // Be careful about overflowing float->int conversions and the
            // like.
            *this = Bounds3<T>();
        else {
            pMin = Point3<T>(b.pMin);
            pMax = Point3<T>(b.pMax);
        }
    }

    LUMIERE_HOST_DEVICE
    bool operator==(const Bounds3<T> &b) const
    {
        return b.pMin == pMin && b.pMax == pMax;
    }

    LUMIERE_HOST_DEVICE
    bool operator!=(const Bounds3<T> &b) const
    {
        return b.pMin != pMin || b.pMax != pMax;
    }

    LUMIERE_HOST_DEVICE bool IntersectP(const Point3f &o, const Vector3f &d, Float tMax = Infinity, Float *hitt0 = nullptr, Float *hitt1 = nullptr) const;
    LUMIERE_HOST_DEVICE bool IntersectP(const Point3f &o, const Vector3f &d, Float tMax,const Vector3f &invDir, const int dirIsNeg[3]) const;

    std::string ToString() const { return LUMIERE_FORMAT("[ {} - {} ]", pMin, pMax); }

public:
    Point3<T> pMin, pMax;
};


using Bounds2f = Bounds2<Float>;
using Bounds2i = Bounds2<int>;
using Bounds3f = Bounds3<Float>;
using Bounds3i = Bounds3<int>;



template <typename T>
LUMIERE_HOST_DEVICE inline Bounds2<T> Union(const Bounds2<T> &b1, const Bounds2<T> &b2)
{
    // Be careful to not run the two-point Bounds constructor.
    Bounds2<T> ret;
    ret.pMin = Min(b1.pMin, b2.pMin);
    ret.pMax = Max(b1.pMax, b2.pMax);
    return ret;
}

template <typename T>
LUMIERE_HOST_DEVICE inline Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T> &b2)
{
    // Be careful to not run the two-point Bounds constructor.
    Bounds2<T> b;
    b.pMin = Max(b1.pMin, b2.pMin);
    b.pMax = Min(b1.pMax, b2.pMax);
    return b;
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool Overlaps(const Bounds2<T> &ba, const Bounds2<T> &bb)
{
    bool x = (ba.pMax.x >= bb.pMin.x) && (ba.pMin.x <= bb.pMax.x);
    bool y = (ba.pMax.y >= bb.pMin.y) && (ba.pMin.y <= bb.pMax.y);
    return (x && y);
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool Inside(const Point2<T> &pt, const Bounds2<T> &b)
{
    return (pt.x >= b.pMin.x && pt.x <= b.pMax.x && pt.y >= b.pMin.y && pt.y <= b.pMax.y);
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool Inside(const Bounds2<T> &ba, const Bounds2<T> &bb)
{
    return (ba.pMin.x >= bb.pMin.x && ba.pMax.x <= bb.pMax.x && ba.pMin.y >= bb.pMin.y && ba.pMax.y <= bb.pMax.y);
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool InsideExclusive(const Point2<T> &pt, const Bounds2<T> &b)
{
    return (pt.x >= b.pMin.x && pt.x < b.pMax.x && pt.y >= b.pMin.y && pt.y < b.pMax.y);
}

template <typename T, typename U>
LUMIERE_HOST_DEVICE inline Bounds2<T> Expand(const Bounds2<T> &b, U delta)
{
    Bounds2<T> ret;
    ret.pMin = b.pMin - Vector2<T>(delta, delta);
    ret.pMax = b.pMax + Vector2<T>(delta, delta);
    return ret;
}


template <typename T>
LUMIERE_HOST_DEVICE inline Bounds3<T> Union(const Bounds3<T> &b, const Point3<T> &p)
{
    Bounds3<T> ret;
    ret.pMin = Min(b.pMin, p);
    ret.pMax = Max(b.pMax, p);
    return ret;
}

template <typename T>
LUMIERE_HOST_DEVICE inline Bounds3<T> Union(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    Bounds3<T> ret;
    ret.pMin = Min(b1.pMin, b2.pMin);
    ret.pMax = Max(b1.pMax, b2.pMax);
    return ret;
}

template <typename T>
LUMIERE_HOST_DEVICE inline Bounds3<T> Intersect(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    Bounds3<T> b;
    b.pMin = Max(b1.pMin, b2.pMin);
    b.pMax = Min(b1.pMax, b2.pMax);
    return b;
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool Overlaps(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
    bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
    bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
    return (x && y && z);
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool Inside(const Point3<T> &p, const Bounds3<T> &b)
{
    return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y && p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool InsideExclusive(const Point3<T> &p, const Bounds3<T> &b)
{
    return (p.x >= b.pMin.x && p.x < b.pMax.x && p.y >= b.pMin.y && p.y < b.pMax.y && p.z >= b.pMin.z && p.z < b.pMax.z);
}

template <typename T, typename U>
LUMIERE_HOST_DEVICE inline auto DistanceSquared(const Point3<T> &p, const Bounds3<U> &b)
{
    using TDist = decltype(T{} - U{});
    TDist dx = std::max<TDist>({0, b.pMin.x - p.x, p.x - b.pMax.x});
    TDist dy = std::max<TDist>({0, b.pMin.y - p.y, p.y - b.pMax.y});
    TDist dz = std::max<TDist>({0, b.pMin.z - p.z, p.z - b.pMax.z});
    return Sqr(dx) + Sqr(dy) + Sqr(dz);
}

template <typename T, typename U>
LUMIERE_HOST_DEVICE inline auto Distance(const Point3<T> &p, const Bounds3<U> &b)
{
    auto dist2 = DistanceSquared(p, b);
    using TDist = typename TupleLength<decltype(dist2)>::type;
    return std::sqrt(TDist(dist2));
}

template <typename T, typename U>
LUMIERE_HOST_DEVICE inline Bounds3<T> Expand(const Bounds3<T> &b, U delta)
{
    Bounds3<T> ret;
    ret.pMin = b.pMin - Vector3<T>(delta, delta, delta);
    ret.pMax = b.pMax + Vector3<T>(delta, delta, delta);
    return ret;
}

/*
template <typename T>
LUMIERE_HOST_DEVICE inline bool Bounds3<T>::IntersectP(const Point3f &o, const Vector3f &d, Float tMax, Float *hitt0, Float *hitt1) const
{
    Float t0 = 0, t1 = tMax;
    for (int i = 0; i < 3; ++i) {
        // Update interval for _i_th bounding box slab
        Float invRayDir = 1 / d[i];
        Float tNear = (pMin[i] - o[i]) * invRayDir;
        Float tFar = (pMax[i] - o[i]) * invRayDir;
        // Update parametric interval from slab intersection $t$ values
        if (tNear > tFar)
            pstd::swap(tNear, tFar);
        // Update _tFar_ to ensure robust ray--bounds intersection
        tFar *= 1 + 2 * gamma(3);

        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1)
            return false;
    }
    if (hitt0)
        *hitt0 = t0;
    if (hitt1)
        *hitt1 = t1;
    return true;
}

template <typename T>
LUMIERE_HOST_DEVICE inline bool Bounds3<T>::IntersectP(const Point3f &o, const Vector3f &d, Float raytMax, const Vector3f &invDir, const int dirIsNeg[3]) const
{
    const Bounds3f &bounds = *this;
    // Check for ray intersection against $x$ and $y$ slabs
    Float tMin = (bounds[dirIsNeg[0]].x - o.x) * invDir.x;
    Float tMax = (bounds[1 - dirIsNeg[0]].x - o.x) * invDir.x;
    Float tyMin = (bounds[dirIsNeg[1]].y - o.y) * invDir.y;
    Float tyMax = (bounds[1 - dirIsNeg[1]].y - o.y) * invDir.y;
    // Update _tMax_ and _tyMax_ to ensure robust bounds intersection
    tMax *= 1 + 2 * gamma(3);
    tyMax *= 1 + 2 * gamma(3);

    if (tMin > tyMax || tyMin > tMax)
        return false;
    if (tyMin > tMin)
        tMin = tyMin;
    if (tyMax < tMax)
        tMax = tyMax;

    // Check for ray intersection against $z$ slab
    Float tzMin = (bounds[dirIsNeg[2]].z - o.z) * invDir.z;
    Float tzMax = (bounds[1 - dirIsNeg[2]].z - o.z) * invDir.z;
    // Update _tzMax_ to ensure robust bounds intersection
    tzMax *= 1 + 2 * gamma(3);

    if (tMin > tzMax || tzMin > tMax)
        return false;
    if (tzMin > tMin)
        tMin = tzMin;
    if (tzMax < tMax)
        tMax = tzMax;

    return (tMin < raytMax) && (tMax > 0);
}
*/

END_LUMIERE_NAMESPACE