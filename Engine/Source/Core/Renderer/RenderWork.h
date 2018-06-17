#pragma once

#include "Core/Filmic/filmic_fwd.h"

namespace ph
{

class Scene;
class Camera;
class Integrator;
class SampleGenerator;

class RenderWork final
{
public:
	const Scene*          scene;
	const Camera*         camera;
	const Integrator*     integrator;
	SampleGenerator*      sampleGenerator;
	SpectralSamplingFilm* film;

	inline RenderWork(const Scene* const          scene,
	                  const Camera* const         camera,
	                  const Integrator* const     integrator,
	                  SampleGenerator* const      sampleGenerator,
	                  SpectralSamplingFilm* const film) :
		scene(scene), 
		camera(camera), 
		integrator(integrator), 
		sampleGenerator(sampleGenerator),
		film(film)
	{}

	inline RenderWork() :
		RenderWork(nullptr, nullptr, nullptr, nullptr, nullptr)
	{}
};

}// end namespace ph