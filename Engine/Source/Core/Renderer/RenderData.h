#pragma once

namespace ph
{

class Scene;
class Camera;
class Integrator;
class SampleGenerator;
class Film;

class RenderData final
{
public:
	const Scene*      scene;
	const Camera*     camera;
	const Integrator* integrator;
	SampleGenerator*  sampleGenerator;
	Film*             film;

	inline RenderData(const Scene* const      scene, 
	                  const Camera* const     camera,
	                  const Integrator* const integrator,
	                  SampleGenerator* const  sampleGenerator,
	                  Film* const             film) :
		scene(scene), 
		camera(camera), 
		integrator(integrator), 
		sampleGenerator(sampleGenerator),
		film(film)
	{

	}

	inline RenderData() :
		RenderData(nullptr, nullptr, nullptr, nullptr, nullptr)
	{

	}
};

}// end namespace ph