#pragma once

namespace ph
{

enum class ETypeCategory
{
	// world command related types
	//
	REF_GEOMETRY, 
	REF_MATERIAL,
	REF_MOTION,
	REF_LIGHT_SOURCE, 
	REF_ACTOR, 
	REF_IMAGE,
	REF_FRAME_PROCESSOR,

	// core command related types
	//
	REF_CAMERA,
	REF_FILM,
	REF_INTEGRATOR,
	REF_SAMPLE_GENERATOR,
	REF_RENDERER,
	
	// special values
	//
	MIN         = REF_GEOMETRY,
	MAX         = REF_RENDERER,
	UNSPECIFIED = MAX + 1
};

}// end namespace ph