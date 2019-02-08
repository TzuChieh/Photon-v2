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

using Vector2R = TVector2<real>;
using Vector2F = TVector2<float32>;
using Vector2D = TVector2<float64>;
using Vector2S = TVector2<std::size_t>;

using Vector3R = TVector3<real>;
using Vector3F = TVector3<float32>;
using Vector3D = TVector3<float64>;
using Vector3S = TVector3<std::size_t>;

using QuaternionR = TQuaternion<real>;
using QuaternionF = TQuaternion<float32>;
using QuaternionD = TQuaternion<float64>;

using Matrix2R = TMatrix2<real>;
using Matrix2F = TMatrix2<float32>;
using Matrix2D = TMatrix2<float64>;

using Matrix4R = TMatrix4<real>;
using Matrix4F = TMatrix4<float32>;
using Matrix4D = TMatrix4<float64>;

}// end namespace ph