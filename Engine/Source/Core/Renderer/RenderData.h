#pragma once

namespace ph
{

class Scene;
class Camera;

class RenderData final
{
public:
	const Scene*  scene;
	const Camera* camera;

	inline RenderData(const Scene* const scene, 
	                  const Camera* const camera) :
		scene(scene), camera(camera)
	{

	}
};

}// end namespace ph