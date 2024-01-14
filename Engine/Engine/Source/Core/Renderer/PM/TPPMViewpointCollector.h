#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TViewpoint.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <vector>
#include <type_traits>
#include <utility>

namespace ph
{

template<CViewpoint Viewpoint>
class TPPMViewpointCollector : public TViewPathHandler<TPPMViewpointCollector<Viewpoint>>
{
	static_assert(std::is_base_of_v<TViewpoint<Viewpoint>, Viewpoint>);

public:
	TPPMViewpointCollector(
		std::size_t           maxViewpointDepth,
		real                  initialKernelRadius);

	bool impl_onReceiverSampleStart(
		const math::Vector2D& rasterCoord,
		const math::Vector2S& sampleIndex,
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

template<CViewpoint Viewpoint>
inline TPPMViewpointCollector<Viewpoint>::TPPMViewpointCollector(
	const std::size_t     maxViewpointDepth,
	const real            initialKernelRadius)

	: m_viewpoints              ()
	, m_maxViewpointDepth       (maxViewpointDepth)
	, m_initialKernelRadius     (initialKernelRadius)
	, m_viewpoint               ()
	, m_receiverSampleViewpoints(0)
{
	PH_ASSERT_GE(maxViewpointDepth, 1);
	PH_ASSERT_GT(initialKernelRadius, 0.0_r);
}

template<CViewpoint Viewpoint>
inline bool TPPMViewpointCollector<Viewpoint>::impl_onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const math::Vector2S& sampleIndex,
	const math::Spectrum& pathThroughput)
{
	if(pathThroughput.isZero())
	{
		// TODO: should we add a 0-contribution viewpoint?
		return false;
	}

	if constexpr(Viewpoint::template has<EViewpointData::RasterCoord>())
	{
		m_viewpoint.template set<EViewpointData::RasterCoord>(rasterCoord);
	}
	if constexpr(Viewpoint::template has<EViewpointData::Radius>())
	{
		m_viewpoint.template set<EViewpointData::Radius>(m_initialKernelRadius);
	}
	if constexpr(Viewpoint::template has<EViewpointData::NumPhotons>())
	{
		m_viewpoint.template set<EViewpointData::NumPhotons>(0.0_r);
	}
	if constexpr(Viewpoint::template has<EViewpointData::Tau>())
	{
		m_viewpoint.template set<EViewpointData::Tau>(math::Spectrum(0));
	}

	m_receiverSampleViewpoints = 0;

	return true;
}

template<CViewpoint Viewpoint>
inline auto TPPMViewpointCollector<Viewpoint>::impl_onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy
{
	const PrimitiveMetadata* const metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* const     optics   = metadata->getSurface().getOptics();

	// Check if there's any non-delta elemental
	SurfacePhenomena phenomena = optics->getAllPhenomena();
	phenomena.turnOff({
		ESurfacePhenomenon::DeltaReflection,
		ESurfacePhenomenon::DeltaTransmission});
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
	       ESurfacePhenomenon::DeltaReflection,
	       ESurfacePhenomenon::DeltaTransmission}))
	{
		return ViewPathTracingPolicy().
			traceBranchedPathFor(SurfacePhenomena({
				ESurfacePhenomenon::DeltaReflection,
				ESurfacePhenomenon::DeltaTransmission})).
			useRussianRoulette(false);
	}
	else
	{
		PH_ASSERT_LE(pathLength, m_maxViewpointDepth);

		return ViewPathTracingPolicy().kill();
	}
}

template<CViewpoint Viewpoint>
inline void TPPMViewpointCollector<Viewpoint>::impl_onReceiverSampleEnd()
{
	if(m_receiverSampleViewpoints > 0)
	{
		// Normalize current receiver sample's path throughput.
		if constexpr(Viewpoint::template has<EViewpointData::ViewThroughput>())
		{
			for(std::size_t i = m_viewpoints.size() - m_receiverSampleViewpoints; i < m_viewpoints.size(); ++i)
			{
				auto& viewpoint = m_viewpoints[i];

				math::Spectrum pathThroughput = viewpoint.template get<EViewpointData::ViewThroughput>();
				pathThroughput.mulLocal(static_cast<real>(m_receiverSampleViewpoints));
				viewpoint.template set<EViewpointData::ViewThroughput>(pathThroughput);
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

template<CViewpoint Viewpoint>
inline void TPPMViewpointCollector<Viewpoint>::impl_onSampleBatchFinished()
{}

template<CViewpoint Viewpoint>
std::vector<Viewpoint> TPPMViewpointCollector<Viewpoint>::claimViewpoints()
{
	return std::move(m_viewpoints);
}

template<CViewpoint Viewpoint>
void TPPMViewpointCollector<Viewpoint>::addViewpoint(
	const SurfaceHit&     surfaceHit,
	const math::Vector3R& viewDir,
	const math::Spectrum& pathThroughput)
{
	if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
	{
		const PrimitiveMetadata* const metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
		PH_ASSERT(metadata);

		math::Spectrum viewRadiance(0);
		if(metadata->getSurface().getEmitter())
		{
			metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &viewRadiance);
		}
		m_viewpoint.template set<EViewpointData::ViewRadiance>(viewRadiance);
	}

	if constexpr(Viewpoint::template has<EViewpointData::SurfaceHit>())
	{
		m_viewpoint.template set<EViewpointData::SurfaceHit>(surfaceHit);
	}
	if constexpr(Viewpoint::template has<EViewpointData::ViewThroughput>())
	{
		m_viewpoint.template set<EViewpointData::ViewThroughput>(pathThroughput);
	}
	if constexpr(Viewpoint::template has<EViewpointData::ViewDir>())
	{
		m_viewpoint.template set<EViewpointData::ViewDir>(viewDir);
	}

	m_viewpoints.push_back(m_viewpoint);
	++m_receiverSampleViewpoints;
}

}// end namespace ph
