#pragma once
#include "Math/LumiereMath.h"

BEGIN_LUMIERE_NAMESPACE

class Plane {
public:
    Plane() = default;
    ~Plane() = default;
    Plane(float A, float B, float C, float D);
    Plane(const Normal3f& normal, const Point3f& point);
    float distanceToPoint(const Point3f& point) const;

private:
    float mA = 0.0f;
    float mB = 0.0f;
    float mC = 0.0f;
    float mD = 0.0f;
};

END_LUMIERE_NAMESPACE
