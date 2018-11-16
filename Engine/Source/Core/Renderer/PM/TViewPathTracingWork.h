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
		const Region& filmRegion);

private:
	void doWork() override;

	ViewPathHandler* m_handler;

	const Scene*     m_scene;
	const Camera*    m_camera;
	SampleGenerator* m_sampleGenerator;
	Region m_filmRegion;

	void traceViewPath(
		Ray tracingRay, 
		SpectralStrength pathThroughput,
		std::size_t pathLength);

	void traceElementallyBranchedPath(
		const ViewPathTracingPolicy& policy,
		const Vector3R& V,
		const Vector3R& N,
		const SurfaceHit& surfaceHit,
		const SpectralStrength& pathThroughput,
		std::size_t pathLength);
};

}// end namespace ph

#include "Core/Renderer/PM/TViewPathTracingWork.ipp"