#pragma once

namespace ph
{

/*! @brief Common data types used by SDL.
Each listed data type has some form of one to one mapping to a specific runtime type. 
Useful for code that require some determinable runtime behavior.
*/
enum class ESdlDataType
{
	None = 0,

	/*! The `ph::int8` type. */
	Int8,

	/*! The `ph::uint8` type. */
	UInt8,

	/*! The `ph::int16` type. */
	Int16,

	/*! The `ph::uint16` type. */
	UInt16,

	/*! The `ph::int32` type. */
	Int32,

	/*! The `ph::uint32` type. */
	UInt32,

	/*! The `ph::int64` type. */
	Int64,

	/*! The `ph::uint64` type. */
	UInt64,

	/*! The `ph::float32` type. */
	Float32,

	/*! The `ph::float64` type. */
	Float64,

	/*! The `bool` type. */
	Bool,

	/*! The `std::string` type. */
	String,

	/*! The `ph::Path` type. */
	Path,

	/*! The `math::Spectrum` type. */
	Spectrum,

	/*! Enum types. Specific `enum class` would require reflection from the enum name. */
	Enum,

	/*! `The ph::Geometry` type. */
	Geometry,

	/*! `The ph::Material` type. */
	Material,

	/*! `The ph::MotionSource` type. */
	Motion,

	/*! `The ph::LightSource` type. */
	LightSource,

	/*! `The ph::Actor` type. */
	Actor,

	/*! `The ph::Image` type. */
	Image,

	/*! `The ph::FrameProcessor` type. */
	FrameProcessor,

	/*! `The ph::Observer` type. */
	Observer,

	/*! `The ph::SampleSource` type. */
	SampleSource,

	/*! `The ph::Visualizer` type. */
	Visualizer,

	/*! `The ph::Option` type. */
	Option,

	SIZE
};

}// end namespace ph
