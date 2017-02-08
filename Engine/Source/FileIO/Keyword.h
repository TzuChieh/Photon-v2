#pragma once

#include <string>

namespace ph
{

class Keyword final
{
public:
	// value types
	static const std::string TYPENAME_STRING;
	static const std::string TYPENAME_INTEGER;
	static const std::string TYPENAME_REAL;
	static const std::string TYPENAME_VECTOR3R;
	static const std::string TYPENAME_QUATERNIONR;
	static const std::string TYPENAME_VECTOR3R_ARRAY;

	// reference types
	static const std::string TYPENAME_GEOMETRY;
	static const std::string TYPENAME_TEXTURE;
	static const std::string TYPENAME_MATERIAL;
	static const std::string TYPENAME_LIGHTSOURCE;
	static const std::string TYPENAME_ACTOR_MODEL;
	static const std::string TYPENAME_ACTOR_LIGHT;
};

}// end namespace ph