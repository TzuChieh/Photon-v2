#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/Viewpoint.h"

#include <cstddef>
#include <memory>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;

class RayTracingWork : public RenderWork
{
public:
	RayTracingWork(
		const Scene* scene,
		const Camera* camera,
		std::unique_ptr<SampleGenerator> sampleGenerator,
		Viewpoint* viewpointBuffer, 
		std::size_t numViewpoints);

	void doWork() override;

private:
	const Scene* m_scene;
	const Camera* m_camera;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	Viewpoint* m_viewpointBuffer;
	std::size_t m_numViewpoints;
};

}// end namespace ph