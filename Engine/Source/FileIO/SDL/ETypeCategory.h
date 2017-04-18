#pragma once

namespace ph
{

enum class ETypeCategory
{
	UNSPECIFIED, 
	PRIMITIVE, 
	REF_GEOMETRY, 
	REF_MATERIAL, 
	REF_LIGHT_SOURCE, 
	REF_ACTOR
};

}// end namespace ph