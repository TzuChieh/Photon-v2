#pragma once

#include "Core/Renderer/RenderProgress.h"

namespace ph
{

class Integrator;
class Renderer;

class RenderWork final
{
public:
	const Integrator* integrator;
	RenderProgress    progress;

	inline RenderWork(
		Renderer* const         renderer, 
		const Integrator* const integrator) :
		integrator(integrator),
		progress(renderer)
	{}

	inline RenderWork() :
		RenderWork(nullptr)
	{}
};

}// end namespace ph