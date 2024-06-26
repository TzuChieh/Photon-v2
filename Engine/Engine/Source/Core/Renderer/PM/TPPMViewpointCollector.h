#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TViewpoint.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Core/SurfaceHit.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/photon_map_light_transport.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <vector>
#include <type_traits>
#include <utility>

namespace ph { class Scene; }

namespace ph
{

/*!
The viewpoint collector for PPM radiance evaluator.
*/
template<CViewpoint Viewpoint, CPhoton Photon>
class TPPMViewpointCollector : public TViewPathHandler<TPPMViewpointCollector<Viewpoint, Photon>>
{
	static_assert(std::is_base_of_v<TViewpoint<Viewpoint>, Viewpoint>);
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

public:
	TPPMViewpointCollector(
		std::size_t                   maxViewpointDepth,
		real                          initialKernelRadius,
		const TPhotonMapInfo<Photon>& photonMapInfo,
		const Scene*                  scene);

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
	void addViewpoint(
		const SurfaceHit& surfaceHit,
		const math::Vector3R& viewDir,
		const math::Spectrum& pathThroughput);

	std::vector<Viewpoint> m_viewpoints;
	std::size_t            m_maxViewpointDepth;
	real                   m_initialKernelRadius;
	TPhotonMapInfo<Photon> m_photonMapInfo;
	const Scene*           m_scene;

	Viewpoint              m_viewpoint;
	math::Spectrum         m_viewRadiance;
	std::size_t            m_numBranchedPathViewpoints;
};

// In-header Implementations:

template<CViewpoint Viewpoint, CPhoton Photon>
inline TPPMViewpointCollector<Viewpoint, Photon>::TPPMViewpointCollector(
	const std::size_t             maxViewpointDepth,
	const real                    initialKernelRadius,
	const TPhotonMapInfo<Photon>& photonMapInfo,
	const Scene* const            scene)

	: m_viewpoints               ()
	, m_maxViewpointDepth        (maxViewpointDepth)
	, m_initialKernelRadius      (initialKernelRadius)
	, m_photonMapInfo            (photonMapInfo)
	, m_scene                    (scene)

	, m_viewpoint                ()
	, m_viewRadiance             ()
	, m_numBranchedPathViewpoints(0)
{
	PH_ASSERT_GE(maxViewpointDepth, 1);
	PH_ASSERT_GT(initialKernelRadius, 0.0_r);
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline bool TPPMViewpointCollector<Viewpoint, Photon>::impl_onReceiverSampleStart(
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
	if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
	{
		m_viewpoint.template set<EViewpointData::ViewRadiance>(math::Spectrum(0));
	}

	m_viewRadiance = math::Spectrum(0);
	m_numBranchedPathViewpoints = 0;

	return true;
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline auto TPPMViewpointCollector<Viewpoint, Photon>::impl_onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy
{
	const SurfaceOptics* optics = surfaceHit.getSurfaceOptics();
	if(!optics)
	{
		return ViewPathTracingPolicy().kill();
	}

	PH_ASSERT_LE(pathLength, m_maxViewpointDepth);

	if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
	{
		const auto unaccountedEnergy = estimate_certainly_lost_energy(
			pathLength,
			surfaceHit,
			pathThroughput,
			m_photonMapInfo,
			m_scene);
		m_viewRadiance += unaccountedEnergy;
	}

	// Below we explicitly list each possible delta phenomenon to emphasize one point: we are
	// not using RR, the number of added viewpoints can grow exponentially, and the base is number
	// of phenomena we list (exponent = view path depth).

	// Add viewpoint if there is any non-delta elemental or max viewpoint depth reached.
	// NOTE: Also merges on glossy! This is just a reference implementation and this can simplify
	// the logic. Can also merge on delta elemental when max viewpoint depth reached (in this case
	// some energy is lost due to the view point depth limit).
	if(pathLength == m_maxViewpointDepth || optics->getAllPhenomena().hasNone({
		ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission}))
	{
		if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
		{
			const auto unaccountedEnergy = estimate_lost_energy_for_merging(
				pathLength,
				surfaceHit,
				pathThroughput,
				m_photonMapInfo,
				m_scene);
			m_viewRadiance += unaccountedEnergy;
		}

		// If on delta surface when max view depth is reached, the viewpoint is for the view
		// radiance only. Could be more efficient if we use a separate film or render pass for that.
		addViewpoint(
			surfaceHit, 
			surfaceHit.getIncidentRay().getDir().mul(-1),
			pathThroughput);

		return ViewPathTracingPolicy().kill();
	}
	else
	{
		PH_ASSERT(optics->getAllPhenomena().hasAny({
			ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission}));

		if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
		{
			const auto unaccountedEnergy = estimate_lost_energy_for_extending(
				pathLength,
				surfaceHit,
				pathThroughput,
				m_photonMapInfo,
				m_scene);
			m_viewRadiance += unaccountedEnergy;
		}

		return ViewPathTracingPolicy().
			traceBranchedPathFor(SurfacePhenomena({
				ESurfacePhenomenon::DeltaReflection, ESurfacePhenomenon::DeltaTransmission})).
			useRussianRoulette(false);
	}
}

template<CViewpoint Viewpoint, CPhoton Photon>
inline void TPPMViewpointCollector<Viewpoint, Photon>::impl_onReceiverSampleEnd()
{
	if(m_numBranchedPathViewpoints > 0)
	{
		// For each viewpoint gathered on this single receiver sample
		for(std::size_t i = m_viewpoints.size() - m_numBranchedPathViewpoints; i < m_viewpoints.size(); ++i)
		{
			auto& viewpoint = m_viewpoints[i];

			// Normalize current receiver sample's path throughput (since no RR is used
			// and each view path carries an independent component of the total energy)
			if constexpr(Viewpoint::template has<EViewpointData::ViewThroughput>())
			{
				math::Spectrum pathThroughput = viewpoint.template get<EViewpointData::ViewThroughput>();
				pathThroughput.mulLocal(static_cast<real>(m_numBranchedPathViewpoints));
				viewpoint.template set<EViewpointData::ViewThroughput>(pathThroughput);
			}

			// `m_viewRadiance` is already properly weighted and accumulated
			if constexpr(Viewpoint::template has<EViewpointData::ViewRadiance>())
			{
				viewpoint.template set<EViewpointData::ViewRadiance>(m_viewRadiance);
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

template<CViewpoint Viewpoint, CPhoton Photon>
inline void TPPMViewpointCollector<Viewpoint, Photon>::impl_onSampleBatchFinished()
{}

template<CViewpoint Viewpoint, CPhoton Photon>
std::vector<Viewpoint> TPPMViewpointCollector<Viewpoint, Photon>::claimViewpoints()
{
	return std::move(m_viewpoints);
}

template<CViewpoint Viewpoint, CPhoton Photon>
void TPPMViewpointCollector<Viewpoint, Photon>::addViewpoint(
	const SurfaceHit&     surfaceHit,
	const math::Vector3R& viewDir,
	const math::Spectrum& pathThroughput)
{
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
	++m_numBranchedPathViewpoints;
}

}// end namespace ph
