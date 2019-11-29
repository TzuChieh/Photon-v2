#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/Region/Region.h"
#include "Math/TVector2.h"
#include "Core/Quantity/SpectralStrength.h"

#include <cstddef>
#include <type_traits>
#include <vector>
#include <utility>

namespace ph
{

class Scene;
class Camera;
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
		const Camera* camera,
		SampleGenerator* sampleGenerator,
		const Region& filmRegion,
		const math::TVector2<int64>& filmSize);

private:
	void doWork() override;

	ViewPathHandler* m_handler;

	const Scene*     m_scene;
	const Camera*    m_camera;
	SampleGenerator* m_sampleGenerator;
	Region m_filmRegion;
	math::TVector2<int64> m_filmSize;

	void traceViewPath(
		Ray tracingRay, 
		SpectralStrength pathThroughput,
		std::size_t pathLength,
		SampleFlow& sampleFlow);

	void traceElementallyBranchedPath(
		const ViewPathTracingPolicy& policy,
		const math::Vector3R& V,
		const math::Vector3R& N,
		const SurfaceHit& surfaceHit,
		const SpectralStrength& pathThroughput,
		std::size_t pathLength,
		SampleFlow& sampleFlow);
};

}// end namespace ph

#include "Core/Renderer/PM/TViewPathTracingWork.ipp"
