#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PPM/Photon.h"

#include <cstddef>
#include <memory>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;

class PhotonMappingWork : public RenderWork
{
public:
	PhotonMappingWork(
		const Scene* scene,
		const Camera* camera,
		std::unique_ptr<SampleGenerator> sampleGenerator,
		Photon* photonBuffer, 
		std::size_t numPhotons);

	void doWork() override;

private:
	const Scene* m_scene;
	const Camera* m_camera;
	std::unique_ptr<SampleGenerator> m_sampleGenerator;
	Photon* m_photonBuffer;
	std::size_t m_numPhotons;
};

}// end namespace ph