#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/TPhoton.h"

#include <cstddef>
#include <memory>
#include <type_traits>
#include <atomic>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;

template<typename Photon>
class TPhotonMappingWork : public RenderWork
{
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

public:
	TPhotonMappingWork(
		const Scene*     scene,
		const Camera*    camera,
		SampleGenerator* sampleGenerator,
		Photon*          photonBuffer, 
		std::size_t      numPhotons,
		std::size_t*     out_numPhotonPaths);

	void doWork() override;

	std::size_t asyncGetNumTracedPhotons() const;
	std::size_t asyncGetNumElapsedMs() const;

private:
	const Scene*     m_scene;
	const Camera*    m_camera;
	SampleGenerator* m_sampleGenerator;
	Photon*          m_photonBuffer;
	std::size_t      m_numPhotons;
	std::size_t*     m_numPhotonPaths;

	std::atomic_uint64_t m_numTracedPhotons;
	std::atomic_uint32_t m_numElapsedMs;
};

}// end namespace ph

#include "Core/Renderer/PM/TPhotonMappingWork.ipp"