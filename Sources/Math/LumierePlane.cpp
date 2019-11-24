#include "LumierePlane.h"

BEGIN_LUMIERE_NAMESPACE


Plane::Plane(float A, float B, float C, float D) : mA(A), mB(B), mC(C), mD(D)
{

}


Plane::Plane(const Normal3f& normal, const Point3f& point)
{
    Normal3f planeNormal = Normalize(normal);
    mA = planeNormal.x;
    mB = planeNormal.y;
    mC = planeNormal.z;
    mD = -(mA * point.x + mB * point.y + mC * point.z);
}


float Plane::distanceToPoint(const Point3f& point) const
{
    return point.x * mA + point.y * mB + point.z * mC + mD;
}

END_LUMIERE_NAMESPACE
