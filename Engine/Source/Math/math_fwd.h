#pragma once

#include "Common/primitive_type.h"

namespace ph
{

template<typename T>
class TVector3;

template<typename T>
class TQuaternion;

template<typename T>
class TMatrix4;

typedef TVector3<real>    Vector3R;
typedef TVector3<float32> Vector3F;
typedef TVector3<float64> Vector3D;

typedef TQuaternion<real>    QuaternionR;
typedef TQuaternion<float32> QuaternionF;
typedef TQuaternion<float64> QuaternionD;

typedef TMatrix4<real>    Matrix4R;
typedef TMatrix4<float32> Matrix4F;
typedef TMatrix4<float64> Matrix4D;

}// end namespace ph