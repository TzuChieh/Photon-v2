#pragma once

namespace ph
{

/*! @brief Format of the data used by SDL.
Each listed data format tells how a group of data is arranged as well as its meaning.
Useful for code that require some determinable runtime behavior.
*/
enum class ESdlDataFormat
{
	None = 0,

	/*! Scalar or containing a single object only. */
	Single,

	/*! `math::TVector2<?>` types. */
	Vector2,

	/*! `math::TVector3<?>` types. */
	Vector3,

	/*! `math::TQuaternion<?>` types. */
	Quaternion,

	/*! `std::vector<?>` types. */
	Vector,

	/*! `std::vector<math::TVector3<?>>` types. */
	Vector3Vector,

	/*! `std::vector<std::shared_ptr<?>>` types. */
	SharedPtrVector,
};

}// end namespace ph
