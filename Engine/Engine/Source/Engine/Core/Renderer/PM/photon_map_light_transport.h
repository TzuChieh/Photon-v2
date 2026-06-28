#pragma once

#include "Engine/Core/Renderer/PM/TPhoton.h"
#include "Engine/Core/Renderer/PM/TPhotonMap.h"
#include "Engine/Core/Renderer/PM/PMCommonParams.h"
#include "Engine/Core/LTA/SidednessAgreement.h"
#include "Engine/Core/LTA/RussianRoulette.h"
#include "Engine/Core/LTA/DirectLightEstimator.h"
#include "Engine/Core/LTA/IndirectLightEstimator.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <limits>
#include <algorithm>

namespace ph { class Scene; }

namespace ph
{

/*!
Given a valid set of target point's evaluation attributes (`Ng`, `Ns`, `L`, `V`), checks whether
`photon` is usable under the `sidedness` constraint.
*/
template<CPhoton Photon>
inline bool accept_photon_by_surface_topology(
	const Photon& photon,
	const math::Vector3R& Ng,
	const math::Vector3R& Ns,
	const math::Vector3R& L,
	const math::Vector3R& V,
	const lta::SidednessAgreement& sidedness)
{
	if constexpr(Photon::template has<EPhotonData::GeometryNormal>())
	{
		const math::Vector3R photonNg = photon.template get<EPhotonData::GeometryNormal>();
		if(photonNg.dot(Ng) < 0.1_r || // ~> 84.26 deg
		   photonNg.dot(Ns) < 0.2_r || // ~> 78.46 deg
		   !sidedness.isSidednessAgreed(photonNg, Ns, V) ||
		   !sidedness.isSidednessAgreed(photonNg, Ns, L))
		{
			return false;
		}
	}

	return true;
}

template<CPhoton Photon>
class TPhotonMapResidualEnergyEstimator final
{
public:
	/*!
	@param scene The scene that is being rendered.
	@param photonMapInfo Information of the photon map that is being involved in the energy estimation process.
	*/
	inline explicit TPhotonMapResidualEnergyEstimator(
		const Scene* scene,
		const TPhotonMapInfo<Photon>& photonMapInfo)

		: m_scene(scene)
		, m_photonMapInfo(photonMapInfo)
	{
		PH_ASSERT(m_scene);
	}

	/*! @brief Estimate the energy that can never be obtained by utilizing a photon map.
	The estimation is for the current hit point only. To account for lost energy along a path
	with multiple hit points, call this function for each hit point and sum the results.

	Example: If we are already on the primary hit point (view length = 1) and a photon map stores photons
	with path lengths >= 1, then we can never use the photon map to estimate lighting for path length = 1.
	This method can estimate the energy lost in this case.

	@param viewPathLength Current view path length. Each path length calculates an independent component
	of the total energy (in equilibrium).
	@param X Current hit point.
	@param bsdfContext Context for BSDF query.
	@param viewPathThroughput Current view path throughput.
	@param minFullPathLength The minimum length of the full light transport path to consider.
	@param maxFullPathLength The maximum length of the full light transport path to consider (inclusive).
	@return The energy that is lost, properly weighted by `viewPathThroughput`.
	*/
	[[nodiscard]]
	inline math::Spectrum certainlyLostEnergy(
		const std::size_t       viewPathLength,
		const SurfaceHit&       X,
		const BsdfQueryContext& bsdfContext,
		const math::Spectrum&   viewPathThroughput,
		const std::size_t       minFullPathLength = 1,
		const std::size_t       maxFullPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH) const
	{
		const lta::DirectLightEstimator directLight{m_scene, bsdfContext.sidedness};

		math::Spectrum lostEnergy(0);

		PH_ASSERT_GE(viewPathLength, 1);
		PH_ASSERT_GE(minFullPathLength, 1);
		PH_ASSERT_LE(minFullPathLength, maxFullPathLength);

		// Cannot have path length = 1 lighting using only photon map--when we use a photon map, it is
		// at least path length = 2 (can be even longer depending on the settings)
	
		// Never contain 0-bounce photons
		PH_ASSERT_GE(m_photonMapInfo.minPathLength, 1);

		// Path length = 1 (0-bounce) lighting via path tracing (directly sample radiance)
		if(viewPathLength == 1 && X.getMetadata().getSurface().isEmissive() && minFullPathLength == 1)
		{
			PH_ASSERT_IN_RANGE_INCLUSIVE(viewPathLength, minFullPathLength, maxFullPathLength);

			math::Spectrum viewRadiance;
			X.getSurfaceEmitter().evalEmittedEnergy(X, &viewRadiance);
			lostEnergy += viewPathThroughput * viewRadiance;
		}

		// +1 as when we merge view path with photon path, path length is at least increased by 1
		const auto minPathLengthWithPhotonMap = m_photonMapInfo.minPathLength + 1;

		// If we can **never** construct the path length from photon map, use path tracing
		if(viewPathLength + 1 < minPathLengthWithPhotonMap && 
		   minFullPathLength <= viewPathLength + 1 && viewPathLength + 1 <= maxFullPathLength)
		{
			BsdfSampleQuery bsdfSample{bsdfContext, X, -X.getIncidentRay().getDir()};
			math::Spectrum viewRadiance;
			SampleFlow randomFlow;// can be exposed for better quality
			if(directLight.bsdfSampleSurfacePathWithNee(
				bsdfSample,
				randomFlow,
				&viewRadiance))
			{
				lostEnergy += viewPathThroughput * viewRadiance;
			}
		}

		return lostEnergy;
	}

	/*! @brief Estimate the energy that is otherwise lost forever if the path is extended.
	The estimation is for the current hit point only. To account for lost energy along an extended path
	with multiple hit points, call this function for each hit point and sum the results. For a hit point,
	only one of `estimate_lost_energy_for_extending()` and `estimate_lost_energy_for_merging()`
	can be called.
	@param viewPathLength Current view path length. Each path length calculates an independent component
	of the total energy (in equilibrium).
	@param X Current hit point.
	@param bsdfContext Context for BSDF query.
	@param viewPathThroughput Current view path throughput.
	@param directPhenomenaMask Possibly choke throughput on `X` by mask.
	@param minFullPathLength The minimum length of the full light transport path to consider.
	@param maxFullPathLength The maximum length of the full light transport path to consider (inclusive).
	@return The energy that is lost, properly weighted by `viewPathThroughput`.
	*/
	[[nodiscard]]
	inline math::Spectrum lostEnergyForExtending(
		const std::size_t       viewPathLength,
		const SurfaceHit&       X,
		const BsdfQueryContext& bsdfContext,
		const math::Spectrum&   viewPathThroughput,
		const SurfacePhenomena& directPhenomenaMask = ALL_SURFACE_PHENOMENA,
		const std::size_t       minFullPathLength = 1,
		const std::size_t       maxFullPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH) const
	{
		const lta::IndirectLightEstimator indirectLight{
			m_scene,
			directPhenomenaMask,
			ALL_SURFACE_PHENOMENA,
			lta::RussianRoulette{},
			1};// `X` is likely a delta or glossy surface, delay RR slightly

		math::Spectrum lostEnergy(0);

		PH_ASSERT_GE(viewPathLength, 1);
		PH_ASSERT_GE(minFullPathLength, 1);
		PH_ASSERT_LE(minFullPathLength, maxFullPathLength);

		// Cannot have path length = 1 lighting using only photon map--when we use a photon map, it is
		// at least path length = 2 (can be even longer depending on the settings)
	
		// Never contain 0-bounce photons
		PH_ASSERT_GE(m_photonMapInfo.minPathLength, 1);

		// If we extend the view path length from N (current) to N + 1, this means we are not using photon
		// map to approximate lighting for path length = N' = `N + m_photonMapInfo.minPathLength`.
		// We will lose energy for path length = N' if we do nothing. Here we use path tracing to
		// find the energy that would otherwise be lost.
		if(minFullPathLength <= viewPathLength + m_photonMapInfo.minPathLength &&
		   viewPathLength + m_photonMapInfo.minPathLength <= maxFullPathLength)
		{
			math::Spectrum viewRadiance;
			SampleFlow randomFlow;// can be exposed for better quality
			if(indirectLight.bsdfSampleSurfacePathWithNee(
				X, 
				bsdfContext,
				randomFlow,
				m_photonMapInfo.minPathLength,// we are already on view path of length N
				&viewRadiance,
				viewPathThroughput))
			{
				lostEnergy += viewRadiance;
			}
		}

		return lostEnergy;
	}

	/*! @brief Estimate the energy that is otherwise lost forever if the path is merged.
	The estimation is for the current hit point only and is expected to be called when
	the path ended (merged). For a hit point with all phenomena involved, only one of
	`lostEnergyForExtending()` and `lostEnergyForMerging()` can be called. When not merging
	all phenomena, the energy lost should be recovered by calling `lostEnergyForExtending()`
	with a `directPhenomenaMask` for phenomena not accounted.
	@param viewPathLength Current view path length. This function calculates an independent component
	of the total energy (in equilibrium).
	@param X Current hit point.
	@param bsdfContext Context for BSDF query.
	@param viewPathThroughput Current view path throughput.
	@param minFullPathLength The minimum length of the full light transport path to consider.
	@param maxFullPathLength The maximum length of the full light transport path to consider (inclusive).
	@return The energy that is lost, properly weighted by `viewPathThroughput`.
	*/
	[[nodiscard]]
	inline math::Spectrum lostEnergyForMerging(
		const std::size_t       viewPathLength,
		const SurfaceHit&       X,
		const BsdfQueryContext& bsdfContext,
		const math::Spectrum&   viewPathThroughput,
		const std::size_t       minFullPathLength = 1,
		const std::size_t       maxFullPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH) const
	{
		const lta::IndirectLightEstimator indirectLight{
			m_scene,
			ALL_SURFACE_PHENOMENA,
			ALL_SURFACE_PHENOMENA,
			lta::RussianRoulette{},
			0};// the path length is likely long already, do RR immediately

		math::Spectrum lostEnergy(0);

		PH_ASSERT_GE(viewPathLength, 1);
		PH_ASSERT_GE(minFullPathLength, 1);
		PH_ASSERT_LE(minFullPathLength, maxFullPathLength);

		// Never contain 0-bounce photons
		PH_ASSERT_GE(m_photonMapInfo.minPathLength, 1);

		// For path length = N (current), we can construct light transport path lengths with photon map,
		// all at once, for the range
		// 
		// [N_min, N_max] = [`N + m_photonMapInfo.minPathLength`, `N + m_photonMapInfo->maxPathLength`].
		// 
		// For path lengths < N_min, they should be accounted for by `certainlyLostEnergy()` and
		// `lostEnergyForExtending()` already.
		// For all path lengths > N_max, use path tracing, which is done below:

		const auto minLostFullPathLength = viewPathLength + m_photonMapInfo.maxPathLength + 1;

		// Skip if it is practically infinite number of bounces already
		const bool isAlreadyEnoughBounces = 
			minLostFullPathLength > PMCommonParams::DEFAULT_MAX_PATH_LENGTH;

		if(!isAlreadyEnoughBounces && minLostFullPathLength <= maxFullPathLength)
		{
			const auto minLostFullPathLengthClipped = std::max(
				minFullPathLength, minLostFullPathLength);
			PH_ASSERT_GE(minLostFullPathLengthClipped, viewPathLength);

			math::Spectrum viewRadiance;
			SampleFlow randomFlow;// can be exposed for better quality
			if(indirectLight.bsdfSampleSurfacePathWithNee(
				X, 
				bsdfContext,
				randomFlow,
				minLostFullPathLengthClipped - viewPathLength,// we are already on view path of length N
				std::numeric_limits<std::size_t>::max(),
				&viewRadiance,
				viewPathThroughput))
			{
				lostEnergy += viewRadiance;
			}
		}

		return lostEnergy;
	}

private:
	const Scene* m_scene;
	TPhotonMapInfo<Photon> m_photonMapInfo;
};

}// end namespace ph
