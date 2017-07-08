#pragma once

#include "Common/primitive_type.h"

namespace ph
{

template<typename T>
class TVector3;

template<typename T>
class TPoint3;

template<typename T>
class TOrientation3;

template<typename T>
class TIndexedVector3;

template<typename T>
class TQuaternion;

template<typename T>
class TMatrix4;

typedef TVector3<real>    Vector3R;
typedef TVector3<float32> Vector3F;
typedef TVector3<float64> Vector3D;

typedef TPoint3<real>    Point3R;
typedef TPoint3<float32> Point3F;
typedef TPoint3<float64> Point3D;

typedef TOrientation3<real>    Orientation3R;
typedef TOrientation3<float32> Orientation3F;
typedef TOrientation3<float64> Orientation3D;

typedef TIndexedVector3<real>    IndexedVector3R;
typedef TIndexedVector3<float32> IndexedVector3F;
typedef TIndexedVector3<float64> IndexedVector3D;

typedef TQuaternion<real>    QuaternionR;
typedef TQuaternion<float32> QuaternionF;
typedef TQuaternion<float64> QuaternionD;

typedef TMatrix4<real>    Matrix4R;
typedef TMatrix4<float32> Matrix4F;
typedef TMatrix4<float64> Matrix4D;

}// end namespace ph