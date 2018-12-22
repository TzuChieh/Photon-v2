#pragma once

#include "Common/primitive_type.h"

#include <cstddef>

namespace ph
{

template<typename T>
class TVector2;

template<typename T>
class TVector3;

template<typename T>
class TIndexedVector3;

template<typename T>
class TQuaternion;

template<typename T>
class TMatrix2;

template<typename T>
class TMatrix4;

template<typename T, std::size_t N>
class TArithmeticArray;

typedef TVector2<real>        Vector2R;
typedef TVector2<float32>     Vector2F;
typedef TVector2<float64>     Vector2D;
typedef TVector2<std::size_t> Vector2S;

typedef TVector3<real>        Vector3R;
typedef TVector3<float32>     Vector3F;
typedef TVector3<float64>     Vector3D;
typedef TVector3<std::size_t> Vector3S;

typedef TQuaternion<real>    QuaternionR;
typedef TQuaternion<float32> QuaternionF;
typedef TQuaternion<float64> QuaternionD;

typedef TMatrix2<real>    Matrix2R;
typedef TMatrix2<float32> Matrix2F;
typedef TMatrix2<float64> Matrix2D;

typedef TMatrix4<real>    Matrix4R;
typedef TMatrix4<float32> Matrix4F;
typedef TMatrix4<float64> Matrix4D;

}// end namespace ph