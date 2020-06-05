#pragma once

namespace ph
{

enum class ETypeCategory
{
	// World command related types
	REF_GEOMETRY, 
	REF_MATERIAL,
	REF_MOTION,
	REF_LIGHT_SOURCE, 
	REF_ACTOR, 
	REF_IMAGE,
	REF_FRAME_PROCESSOR,

	// Core command related types
	REF_RECEIVER,
	REF_SAMPLE_GENERATOR,
	REF_RENDERER,
	REF_OPTION,
	
	// Special values
	MIN         = REF_GEOMETRY,
	MAX         = REF_OPTION,
	UNSPECIFIED = MAX + 1
};

}// end namespace ph
