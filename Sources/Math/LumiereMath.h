#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "LumiereColor.h"
#include "LumiereGeometry.h"
#include "LumiereVector.h"

BEGIN_LUMIERE_NAMESPACE

#if LUMIERE_USE_GLM

using Vector2u = glm::uvec2;
using Vector2i = glm::ivec2;
using Vector2f = glm::vec2;
using Vector3f = glm::vec3;
using Vector4f = glm::vec4;

using Matrix3x3 = glm::mat3;
using Matrix4x4 = glm::mat4;

using Point2i = glm::ivec2;
using Point2f = glm::vec2;
using Point3f = glm::vec3;
using Position = glm::vec3;
using Normal3f = glm::vec3;

#else

using Matrix3x3 = glm::mat3;
using Matrix4x4 = glm::mat4;

using Transform = glm::mat4;

#endif


#ifdef LUMIERE_USE_GLM

template <typename T>
inline auto GetRawValue(const T& value) -> decltype(glm::value_ptr(value))
{
    return glm::value_ptr(value);
}


inline Vector3f Normalize(const Vector3f& vector)
{
    return glm::normalize(vector);
}


inline Vector3f Cross(const Vector3f& lhs, const Vector3f& rhs)
{
    return glm::cross(lhs, rhs);
}

inline float Dot(const Vector3f& lhs, const Vector3f& rhs)
{
    return glm::dot(lhs, rhs);
}


inline Matrix4x4 CalculateViewMatrix(const Point3f& eyePosition, const Point3f& lookAt, const Vector3f& upVector)
{
    return glm::lookAt(eyePosition, lookAt, upVector);
}


inline Matrix4x4 CalculateProjectionMatrix(float FOVy, float aspect, float nearClipDistance, float farClipDistance)
{
    return glm::perspective(FOVy, aspect, nearClipDistance, farClipDistance);
}

inline float Length(const Normal3f& normal)
{
    return glm::length(normal);
}

#else


#endif


inline float ConvertDegreeToRadians(float angle)
{
    return glm::radians(angle);
}

END_LUMIERE_NAMESPACE
