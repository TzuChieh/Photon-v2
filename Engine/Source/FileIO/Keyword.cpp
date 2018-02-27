#include "FileIO/Keyword.h"

namespace ph
{

// value types
const std::string Keyword::TYPENAME_STRING         = "string";
const std::string Keyword::TYPENAME_INTEGER        = "integer";
const std::string Keyword::TYPENAME_REAL           = "real";
const std::string Keyword::TYPENAME_VECTOR3R       = "vector3r";
const std::string Keyword::TYPENAME_QUATERNIONR    = "quaternionR";
const std::string Keyword::TYPENAME_REAL_ARRAY     = "real-array";
const std::string Keyword::TYPENAME_VECTOR3R_ARRAY = "vector3r-array";

// reference types
const std::string Keyword::TYPENAME_GEOMETRY    = "geometry";
const std::string Keyword::TYPENAME_TEXTURE     = "texture";
const std::string Keyword::TYPENAME_MATERIAL    = "material";
const std::string Keyword::TYPENAME_LIGHTSOURCE = "light-source";
const std::string Keyword::TYPENAME_ACTOR_MODEL = "actor-model";
const std::string Keyword::TYPENAME_ACTOR_LIGHT = "actor-light";

}// end namespace ph