#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Math/Color/Spectrum.h"
#include "Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <memory>
#include <type_traits>

namespace ph
{

class Scene;
class Receiver;
class SampleGenerator;
class SurfaceHit;
class Ray;
class PMAtomicStatistics;

template<CPhoton Photon>
class TPhotonPathTracingWork : public RenderWork
{
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

public:
	TPhotonPathTracingWork(
		const Scene*     scene,
		const Receiver*  receiver,
		SampleGenerator* sampleGenerator,
		TSpan<Photon>    photonBuffer,
		uint32           minPhotonBounces = 1,
		uint32           maxPhotonBounces = 16384);

	void setStatistics(PMAtomicStatistics* statistics);

	std::size_t numPhotonPaths() const;

private:
	void doWork() override;

	static Photon makePhoton(
		const SurfaceHit&     surfaceHit, 
		const math::Spectrum& throughputRadiance,
		const Ray&            tracingRay);

	const Scene*        m_scene;
	const Receiver*     m_receiver;
	SampleGenerator*    m_sampleGenerator;
	TSpan<Photon>       m_photonBuffer;
	uint32              m_minPhotonBounces;
	uint32              m_maxPhotonBounces;
	std::size_t         m_numPhotonPaths;
	PMAtomicStatistics* m_statistics;
};

}// end namespace ph

#include "Core/Renderer/PM/TPhotonPathTracingWork.ipp"
