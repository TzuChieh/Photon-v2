#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Scheduler/Region.h"
#include "Math/TVector2.h"
#include "Core/Quantity/Spectrum.h"

#include <cstddef>
#include <type_traits>
#include <vector>
#include <utility>

namespace ph
{

class Scene;
class Receiver;
class SampleGenerator;
class Ray;
class SampleFlow;

template<typename ViewPathHandler>
class TViewPathTracingWork : public RenderWork
{
	static_assert(std::is_base_of_v<TViewPathHandler<ViewPathHandler>, ViewPathHandler>);

public:
	TViewPathTracingWork(
		ViewPathHandler* handler,
		const Scene* scene,
		const Receiver* receiver,
		SampleGenerator* sampleGenerator,
		const Region& rasterRegion);

private:
	void doWork() override;

	ViewPathHandler* m_handler;

	const Scene*     m_scene;
	const Receiver*  m_receiver;
	SampleGenerator* m_sampleGenerator;
	Region m_rasterRegion;

	void traceViewPath(
		Ray tracingRay, 
		Spectrum pathThroughput,
		std::size_t pathLength,
		SampleFlow& sampleFlow);

	void traceElementallyBranchedPath(
		const ViewPathTracingPolicy& policy,
		const math::Vector3R& V,
		const math::Vector3R& N,
		const SurfaceHit& surfaceHit,
		const Spectrum& pathThroughput,
		std::size_t pathLength,
		SampleFlow& sampleFlow);
};

}// end namespace ph

#include "Core/Renderer/PM/TViewPathTracingWork.ipp"
