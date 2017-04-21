#pragma once

namespace ph
{

enum class ETypeCategory
{
	PRIMITIVE, 
	REF_GEOMETRY, 
	REF_MATERIAL, 
	REF_LIGHT_SOURCE, 
	REF_ACTOR, 

	UNSPECIFIED,
	MIN = PRIMITIVE,
	MAX = REF_ACTOR
};

}// end namespace ph