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
#include "Math/Random.h"

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

	bool impl_onReceiverSampleStart(
		const math::Vector2D& rasterCoord,
		const math::Spectrum& pathThroughput);

	auto impl_onPathHitSurface(
		std::size_t           pathLength,
		const SurfaceHit&     surfaceHit,
		const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy;

	void impl_onReceiverSampleEnd();

	void impl_onSampleBatchFinished();

	std::vector<Viewpoint> claimViewpoints();

private:
	std::vector<Viewpoint> m_viewpoints;
	std::size_t            m_maxViewpointDepth;
	real                   m_initialKernelRadius;

	Viewpoint              m_viewpoint;
	std::size_t            m_receiverSampleViewpoints;

	void addViewpoint(
		const SurfaceHit&     surfaceHit, 
		const math::Vector3R& viewDir,
		const math::Spectrum& pathThroughput);
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
inline bool TPPMViewpointCollector<Viewpoint>::impl_onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const math::Spectrum& pathThroughput)
{
	if(pathThroughput.isZero())
	{
		// TODO: should we add a 0-contribution viewpoint?
		return false;
	}

	if constexpr(Viewpoint::template has<EViewpointData::RASTER_COORD>()) {
		m_viewpoint.template set<EViewpointData::RASTER_COORD>(rasterCoord);
	}
	if constexpr(Viewpoint::template has<EViewpointData::RADIUS>()) {
		m_viewpoint.template set<EViewpointData::RADIUS>(m_initialKernelRadius);
	}
	if constexpr(Viewpoint::template has<EViewpointData::NUM_PHOTONS>()) {
		m_viewpoint.template set<EViewpointData::NUM_PHOTONS>(0.0_r);
	}
	if constexpr(Viewpoint::template has<EViewpointData::TAU>()) {
		m_viewpoint.template set<EViewpointData::TAU>(math::Spectrum(0));
	}

	m_receiverSampleViewpoints = 0;

	return true;
}

template<typename Viewpoint>
inline auto TPPMViewpointCollector<Viewpoint>::impl_onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy
{
	const PrimitiveMetadata* const metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* const     optics   = metadata->getSurface().getOptics();

	// check if there's any non-delta elemental
	SurfacePhenomena phenomena = optics->getAllPhenomena();
	phenomena.turnOff({
		ESurfacePhenomenon::DELTA_REFLECTION,
		ESurfacePhenomenon::DELTA_TRANSMISSION});
	if(!phenomena.isEmpty())
	{
		// It is okay to add a viewpoint without specifying which surface
		// elemental is used--since it is impossible for delta elementals
		// to have non-zero contribution in any BSDF evaluation process, so
		// we will not double-count any path throughput.
		addViewpoint(
			surfaceHit, 
			surfaceHit.getIncidentRay().getDirection().mul(-1), 
			pathThroughput);
	}

	if(pathLength < m_maxViewpointDepth && 
	   optics->getAllPhenomena().hasAny({
		ESurfacePhenomenon::DELTA_REFLECTION,
		ESurfacePhenomenon::DELTA_TRANSMISSION}))
	{
		return ViewPathTracingPolicy().
			traceBranchedPathFor(SurfacePhenomena({
				ESurfacePhenomenon::DELTA_REFLECTION,
				ESurfacePhenomenon::DELTA_TRANSMISSION})).
			useRussianRoulette(false);
	}
	else
	{
		PH_ASSERT_LE(pathLength, m_maxViewpointDepth);

		return ViewPathTracingPolicy().kill();
	}
}

template<typename Viewpoint>
inline void TPPMViewpointCollector<Viewpoint>::impl_onReceiverSampleEnd()
{
	if(m_receiverSampleViewpoints > 0)
	{
		// Normalize current receiver sample's path throughput.
		if constexpr(Viewpoint::template has<EViewpointData::VIEW_THROUGHPUT>())
		{
			for(std::size_t i = m_viewpoints.size() - m_receiverSampleViewpoints; i < m_viewpoints.size(); ++i)
			{
				auto& viewpoint = m_viewpoints[i];

				math::Spectrum pathThroughput = viewpoint.template get<EViewpointData::VIEW_THROUGHPUT>();
				pathThroughput.mulLocal(static_cast<real>(m_receiverSampleViewpoints));
				viewpoint.template set<EViewpointData::VIEW_THROUGHPUT>(pathThroughput);
			}
		}
	}
	else
	{
		//// If no viewpoint is found for current receiver sample, we should add an
		//// zero-contribution viewpoint.

		//// HACK
		//m_viewpoint.template set<EViewpointData::RADIUS>(0.0_r);
		//addViewpoint(
		//	SurfaceHit(),
		//	Vector3R(),
		//	Spectrum());
	}
}

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
	const SurfaceHit&     surfaceHit,
	const math::Vector3R& viewDir,
	const math::Spectrum& pathThroughput)
{
	if constexpr(Viewpoint::template has<EViewpointData::VIEW_RADIANCE>())
	{
		const PrimitiveMetadata* const metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
		PH_ASSERT(metadata);

		math::Spectrum viewRadiance(0);
		if(metadata->getSurface().getEmitter())
		{
			metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &viewRadiance);
		}
		m_viewpoint.template set<EViewpointData::VIEW_RADIANCE>(viewRadiance);
	}

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
	++m_receiverSampleViewpoints;
}

}// end namespace ph