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
class Receiver;
class SampleGenerator;
class PMAtomicStatistics;

template<typename Photon>
class TPhotonMappingWork : public RenderWork
{
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

public:
	TPhotonMappingWork(
		const Scene*     scene,
		const Receiver*  receiver,
		SampleGenerator* sampleGenerator,
		Photon*          photonBuffer, 
		std::size_t      numPhotons,
		std::size_t*     out_numPhotonPaths);

	void setStatistics(PMAtomicStatistics* statistics);

private:
	void doWork() override;

	const Scene*        m_scene;
	const Receiver*     m_receiver;
	SampleGenerator*    m_sampleGenerator;
	Photon*             m_photonBuffer;
	std::size_t         m_numPhotons;
	std::size_t*        m_numPhotonPaths;
	PMAtomicStatistics* m_statistics;
};

}// end namespace ph

#include "Core/Renderer/PM/TPhotonMappingWork.ipp"
