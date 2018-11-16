#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TViewpoint.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <vector>
#include <type_traits>
#include <utility>

namespace ph
{

template<typename Viewpoint>
class TPPMViewpointCollector : public TViewPathHandler<TPPMViewpointCollector<Viewpoint>>
{
	static_assert(std::is_base_of_v<TViewpoint<Viewpoint>, Viewpoint>);

public:
	TPPMViewpointCollector(
		std::size_t maxViewpointDepth,
		real        initialKernelRadius);

	bool impl_onCameraSampleStart(
		const Vector2R&         filmNdc,
		const SpectralStrength& pathThroughput);

	auto impl_onPathHitSurface(
		std::size_t             pathLength,
		const SurfaceHit&       surfaceHit,
		const SpectralStrength& pathThroughput) -> ViewPathTracingPolicy;

	void impl_onCameraSampleEnd();

	void impl_onSampleBatchFinished();

	std::vector<Viewpoint> claimViewpoints();

private:
	std::vector<Viewpoint> m_viewpoints;
	std::size_t            m_maxViewpointDepth;
	real                   m_initialKernelRadius;

	Viewpoint              m_viewpoint;
	std::size_t            m_cameraSampleViewpoints;

	void addViewpoint(
		const SurfaceHit&       surfaceHit, 
		SurfaceElemental        elemental, 
		const Vector3R&         viewDir, 
		const SpectralStrength& pathThroughput);
};

// In-header Implementations:

template<typename Viewpoint>
inline TPPMViewpointCollector<Viewpoint>::TPPMViewpointCollector(
	const std::size_t       maxViewpointDepth,
	const real              initialKernelRadius) : 
	m_maxViewpointDepth  (maxViewpointDepth),
	m_initialKernelRadius(initialKernelRadius)
{
	PH_ASSERT_GE(maxViewpointDepth, 1);
	PH_ASSERT_GT(initialKernelRadius, 0.0_r);
}

template<typename Viewpoint>
inline bool TPPMViewpointCollector<Viewpoint>::impl_onCameraSampleStart(
	const Vector2R&         filmNdc,
	const SpectralStrength& pathThroughput)
{
	if(pathThroughput.isZero())
	{
		return false;
	}

	if constexpr(Viewpoint::template has<EViewpointData::FILM_NDC>()) {
		m_viewpoint.template set<EViewpointData::FILM_NDC>(filmNdc);
	}
	if constexpr(Viewpoint::template has<EViewpointData::RADIUS>()) {
		m_viewpoint.template set<EViewpointData::RADIUS>(m_initialKernelRadius);
	}
	if constexpr(Viewpoint::template has<EViewpointData::NUM_PHOTONS>()) {
		m_viewpoint.template set<EViewpointData::NUM_PHOTONS>(0.0_r);
	}
	if constexpr(Viewpoint::template has<EViewpointData::TAU>()) {
		m_viewpoint.template set<EViewpointData::TAU>(SpectralStrength(0));
	}

	m_cameraSampleViewpoints = 0;

	return true;
}

template<typename Viewpoint>
inline auto TPPMViewpointCollector<Viewpoint>::impl_onPathHitSurface(
	const std::size_t       pathLength,
	const SurfaceHit&       surfaceHit,
	const SpectralStrength& pathThroughput) -> ViewPathTracingPolicy
{
	PH_ASSERT_GE(pathLength, 1);

	const PrimitiveMetadata* const metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* const     optics   = metadata->getSurface().getOptics();

	for(SurfaceElemental i = 0; i < optics->numElementals(); ++i)
	{
		
	}

	if(optics->getAllPhenomena().hasNone({
		ESurfacePhenomenon::DELTA_REFLECTION,
		ESurfacePhenomenon::DELTA_TRANSMISSION}))
	{
		addViewpoint(
			surfaceHit, 
			ALL_ELEMENTALS, 
			surfaceHit.getIncidentRay().getDirection().mul(-1), 
			pathThroughput);

		return ViewPathTracingPolicy().kill();
	}
	else
	{


		// TODO

		return ViewPathTracingPolicy().
			traceBranchedPathFor({
				ESurfacePhenomenon::DELTA_REFLECTION,
				ESurfacePhenomenon::DELTA_TRANSMISSION}).
			useRussianRoulette(false);
	}

	return false;
}

template<typename Viewpoint>
inline void TPPMViewpointCollector<Viewpoint>::impl_onCameraSampleEnd()
{}

template<typename Viewpoint>
inline void TPPMViewpointCollector<Viewpoint>::impl_onSampleBatchFinished()
{}

template<typename Viewpoint>
std::vector<Viewpoint> TPPMViewpointCollector<Viewpoint>::claimViewpoints()
{
	return std::move(m_viewpoints);
}

template<typename Viewpoint>
void TPPMViewpointCollector<Viewpoint>::addViewpoint(
	const SurfaceHit&       surfaceHit,
	const SurfaceElemental  elemental,
	const Vector3R&         viewDir,
	const SpectralStrength& pathThroughput)
{
	const PrimitiveMetadata* const metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
	PH_ASSERT(metadata);

	if constexpr(Viewpoint::template has<EViewpointData::VIEW_RADIANCE>())
	{
		SpectralStrength viewRadiance(0);
		if(metadata->getSurface().getEmitter())
		{
			metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &viewRadiance);
			viewRadiance.mulLocal(pathThroughput);
		}
		m_viewpoint.template set<EViewpointData::VIEW_RADIANCE>(viewRadiance);
	}

	const SurfaceOptics* const optics = metadata->getSurface().getOptics();
	PH_ASSERT(optics);

	if constexpr(Viewpoint::template has<EViewpointData::SURFACE_HIT>()) {
		m_viewpoint.template set<EViewpointData::SURFACE_HIT>(surfaceHit);
	}
	if constexpr(Viewpoint::template has<EViewpointData::VIEW_THROUGHPUT>()) {
		m_viewpoint.template set<EViewpointData::VIEW_THROUGHPUT>(pathThroughput);
	}
	if constexpr(Viewpoint::template has<EViewpointData::VIEW_DIR>()) {
		m_viewpoint.template set<EViewpointData::VIEW_DIR>(viewDir);
	}

	m_viewpoints.push_back(m_viewpoint);
	++m_cameraSampleViewpoints;
}

}// end namespace ph