#pragma once

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Format of the data used by SDL.
Each listed data format tells how a group of data is arranged internally as well as its meaning.
Useful for code that require some determinable runtime behavior.
*/
enum class ESdlDataFormat : uint8
{
	None = 0,

	/*! `T` types. Scalar or containing a single object only. */
	Single,

	/*! `math::TVector2<T>` types. */
	Vector2,

	/*! `math::TVector3<T>` types. */
	Vector3,

	/*! `math::TQuaternion<T>` types. */
	Quaternion,

	/*! `std::vector<T>` types. */
	Vector,

	/*! `std::vector<math::TVector2<T>>` types. */
	Vector2Vector,

	/*! `std::vector<math::TVector3<T>>` types. */
	Vector3Vector,

	/*! `std::vector<math::TQuaternion<T>>` types. */
	QuaternionVector,

	/*! `std::vector<std::shared_ptr<T>>` types. */
	SharedPtrVector,

	/*! `std::shared_ptr<T>` types. */
	SharedPointer,

	/*! `std::unique_ptr<T>` types. */
	UniquePointer,

	SIZE
};

}// end namespace ph
