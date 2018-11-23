#pragma once

#include <string_view>

namespace ph
{

class Keyword final
{
public:
	// value types
	constexpr static std::string_view TYPENAME_STRING         = "string";
	constexpr static std::string_view TYPENAME_INTEGER        = "integer";
	constexpr static std::string_view TYPENAME_REAL           = "real";
	constexpr static std::string_view TYPENAME_VECTOR3        = "vector3";
	constexpr static std::string_view TYPENAME_QUATERNION     = "quaternion";
	constexpr static std::string_view TYPENAME_REAL_ARRAY     = "real-array";
	constexpr static std::string_view TYPENAME_VECTOR3_ARRAY  = "vector3-array";

	// reference types
	constexpr static std::string_view TYPENAME_GEOMETRY    = "geometry";
	constexpr static std::string_view TYPENAME_TEXTURE     = "texture";
	constexpr static std::string_view TYPENAME_MATERIAL    = "material";
	constexpr static std::string_view TYPENAME_LIGHTSOURCE = "light-source";
	constexpr static std::string_view TYPENAME_ACTOR_MODEL = "actor-model";
	constexpr static std::string_view TYPENAME_ACTOR_LIGHT = "actor-light";
};

}// end namespace ph