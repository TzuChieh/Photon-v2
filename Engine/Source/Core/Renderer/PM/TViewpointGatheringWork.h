#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/TViewpoint.h"
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

template<typename Viewpoint>
class TViewpointGatheringWork : public RenderWork
{
	static_assert(std::is_base_of_v<TViewpoint<Viewpoint>, Viewpoint>);

public:
	TViewpointGatheringWork(
		const Scene* scene,
		const Camera* camera,
		SampleGenerator* sampleGenerator,
		const Region& filmRegion,
		real kernelRadius);

	std::vector<Viewpoint> claimViewpoints();

private:
	void doWork() override;

	const Scene*     m_scene;
	const Camera*    m_camera;
	SampleGenerator* m_sampleGenerator;
	Region m_filmRegion;
	std::vector<Viewpoint> m_viewpoints;
	real             m_kernelRadius;
	std::size_t m_maxViewpointDepth;

	void gatherViewpointsRecursive(
		const Ray& tracingRay, 
		const Vector2R& filmNdc,
		const SpectralStrength& throughput,
		std::size_t currentViewpointDepth);
};

// In-header Implementations:

template<typename Viewpoint>
inline std::vector<Viewpoint> TViewpointGatheringWork<Viewpoint>::claimViewpoints()
{
	return std::move(m_viewpoints);
}

}// end namespace ph

#include "Core/Renderer/PM/TViewpointGatheringWork.ipp"