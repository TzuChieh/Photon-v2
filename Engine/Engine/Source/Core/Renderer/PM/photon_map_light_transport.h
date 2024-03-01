#pragma once

#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/PMCommonParams.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/LTA/RussianRoulette.h"
#include "Core/LTA/TDirectLightEstimator.h"
#include "Core/LTA/TIndirectLightEstimator.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Math/TVector3.h"
#include "Math/Color/Spectrum.h"

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

/*! @brief Estimate the energy that can never be obtained by utilizing a photon map.
The estimation is for the current hit point only. To account for lost energy along a path
with multiple hit points, call this function for each hit point and sum the results.
@param viewPathLength Current view path length. Each path length calculates an independent component
of the total energy (in equilibrium).
@param X Current hit point.
@param viewPathThroughput Current view path throughput.
@param photonMapInfo Information of the photon map that is being involved in the energy evaluation process.
@param scene The scene that is being rendered.
@param minFullPathLength The minimum length of the full light transport path to consider.
@param maxFullPathLength The maximum length of the full light transport path to consider (inclusive).
@return The energy that is lost, properly weighted by `viewPathThroughput`.
*/
template<CPhoton Photon>
inline math::Spectrum estimate_certainly_lost_energy(
	const std::size_t             viewPathLength,
	const SurfaceHit&             X,
	const math::Spectrum&         viewPathThroughput,
	const TPhotonMapInfo<Photon>& photonMapInfo,
	const Scene*                  scene,
	const std::size_t             minFullPathLength = 1,
	const std::size_t             maxFullPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH)
{
	using DirectLight = lta::TDirectLightEstimator<lta::ESidednessPolicy::Strict>;

	math::Spectrum lostEnergy(0);
	if(!X.getSurfaceOptics())
	{
		return lostEnergy;
	}

	PH_ASSERT_GE(viewPathLength, 1);
	PH_ASSERT_GE(minFullPathLength, 1);
	PH_ASSERT_LE(minFullPathLength, maxFullPathLength);

	// Cannot have path length = 1 lighting using only photon map--when we use a photon map, it is
	// at least path length = 2 (can be even longer depending on the settings)
	
	// Never contain 0-bounce photons
	PH_ASSERT_GE(photonMapInfo.minPathLength, 1);

	// Path length = 1 (0-bounce) lighting via path tracing (directly sample radiance)
	if(viewPathLength == 1 && X.getSurfaceEmitter() && minFullPathLength == 1)
	{
		PH_ASSERT_IN_RANGE_INCLUSIVE(viewPathLength, minFullPathLength, maxFullPathLength);

		math::Spectrum viewRadiance;
		X.getSurfaceEmitter()->evalEmittedRadiance(X, &viewRadiance);
		lostEnergy += viewPathThroughput * viewRadiance;
	}

	// +1 as when we merge view path with photon path, path length is at least increased by 1
	const auto minPathLengthWithPhotonMap = photonMapInfo.minPathLength + 1;

	// If we can **never** construct the path length from photon map, use path tracing
	if(viewPathLength + 1 < minPathLengthWithPhotonMap && 
	   minFullPathLength <= viewPathLength + 1 && viewPathLength + 1 <= maxFullPathLength)
	{
		BsdfSampleQuery bsdfSample;
		bsdfSample.inputs.set(X, -X.getIncidentRay().getDirection());

		math::Spectrum viewRadiance;
		SampleFlow randomFlow;
		if(DirectLight{scene}.bsdfSamplePathWithNee(
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
@param viewPathThroughput Current view path throughput.
@param photonMapInfo Information of the photon map that is being involved in the energy evaluation process.
@param scene The scene that is being rendered.
@param minFullPathLength The minimum length of the full light transport path to consider.
@param maxFullPathLength The maximum length of the full light transport path to consider (inclusive).
@return The energy that is lost, properly weighted by `viewPathThroughput`.
*/
template<CPhoton Photon>
inline math::Spectrum estimate_lost_energy_for_extending(
	const std::size_t             viewPathLength,
	const SurfaceHit&             X,
	const math::Spectrum&         viewPathThroughput,
	const TPhotonMapInfo<Photon>& photonMapInfo,
	const Scene*                  scene,
	const std::size_t             minFullPathLength = 1,
	const std::size_t             maxFullPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH)
{
	using IndirectLight = lta::TIndirectLightEstimator<lta::ESidednessPolicy::Strict>;

	math::Spectrum lostEnergy(0);
	if(!X.getSurfaceOptics())
	{
		return lostEnergy;
	}

	PH_ASSERT_GE(viewPathLength, 1);
	PH_ASSERT_GE(minFullPathLength, 1);
	PH_ASSERT_LE(minFullPathLength, maxFullPathLength);

	// Cannot have path length = 1 lighting using only photon map--when we use a photon map, it is
	// at least path length = 2 (can be even longer depending on the settings)
	
	// Never contain 0-bounce photons
	PH_ASSERT_GE(photonMapInfo.minPathLength, 1);

	// If we extend the view path length from N (current) to N + 1, this means we are not using photon
	// map to approximate lighting for path length = N' = `N + photonMapInfo.minPathLength`.
	// We will lose energy for path length = N' if we do nothing. Here we use path tracing to
	// find the energy that would otherwise be lost.
	if(minFullPathLength <= viewPathLength + photonMapInfo.minPathLength &&
	   viewPathLength + photonMapInfo.minPathLength <= maxFullPathLength)
	{
		math::Spectrum viewRadiance;
		SampleFlow randomFlow;
		if(IndirectLight{scene}.bsdfSamplePathWithNee(
			X, 
			randomFlow,
			photonMapInfo.minPathLength,// we are already on view path of length N
			lta::RussianRoulette{},
			&viewRadiance,
			1,// `X` is likely a delta or glossy surface, delay RR slightly
			viewPathThroughput))
		{
			lostEnergy += viewRadiance;
		}
	}

	return lostEnergy;
}

/*! @brief Estimate the energy that is otherwise lost forever if the path is merged.
The estimation is for the current hit point only and is expected to be called when
the path ended (merged). For a hit point, only one of `estimate_lost_energy_for_extending()`
and `estimate_lost_energy_for_merging()` can be called.
@param viewPathLength Current view path length. This function calculates an independent component
of the total energy (in equilibrium).
@param X Current hit point.
@param viewPathThroughput Current view path throughput.
@param photonMapInfo Information of the photon map that is being involved in the energy evaluation process.
@param scene The scene that is being rendered.
@param minFullPathLength The minimum length of the full light transport path to consider.
@param maxFullPathLength The maximum length of the full light transport path to consider (inclusive).
@return The energy that is lost, properly weighted by `viewPathThroughput`.
*/
template<CPhoton Photon>
inline math::Spectrum estimate_lost_energy_for_merging(
	const std::size_t             viewPathLength,
	const SurfaceHit&             X,
	const math::Spectrum&         viewPathThroughput,
	const TPhotonMapInfo<Photon>& photonMapInfo,
	const Scene*                  scene,
	const std::size_t             minFullPathLength = 1,
	const std::size_t             maxFullPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH)
{
	using IndirectLight = lta::TIndirectLightEstimator<lta::ESidednessPolicy::Strict>;

	math::Spectrum lostEnergy(0);
	if(!X.getSurfaceOptics())
	{
		return lostEnergy;
	}

	PH_ASSERT_GE(viewPathLength, 1);
	PH_ASSERT_GE(minFullPathLength, 1);
	PH_ASSERT_LE(minFullPathLength, maxFullPathLength);

	// Never contain 0-bounce photons
	PH_ASSERT_GE(photonMapInfo.minPathLength, 1);

	// For path length = N (current), we can construct light transport path lengths with photon map,
	// all at once, for the range [N_min, N_max] = 
	// [`N + photonMapInfo.minPathLength`, `N + photonMapInfo.maxPathLength`].
	// For path lengths < N_min, they should be accounted for by `estimate_lost_energy_for_extend()`
	// already. For all path lengths > N_max, use path tracing, which is done below:

	const auto minLostFullPathLength = viewPathLength + photonMapInfo.maxPathLength + 1;

	// Will also skip this if it is practically infinite number of bounces already
	const bool isAlreadyEnoughBounces = 
		minLostFullPathLength > PMCommonParams::DEFAULT_MAX_PATH_LENGTH;

	if(!isAlreadyEnoughBounces && minLostFullPathLength <= maxFullPathLength)
	{
		const auto minLostFullPathLengthClipped = std::max(
			minFullPathLength, minLostFullPathLength);
		PH_ASSERT_GE(minLostFullPathLengthClipped, viewPathLength);

		math::Spectrum viewRadiance;
		SampleFlow randomFlow;
		if(IndirectLight{scene}.bsdfSamplePathWithNee(
			X, 
			randomFlow,
			minLostFullPathLengthClipped - viewPathLength,// we are already on view path of length N
			std::numeric_limits<std::size_t>::max(),
			lta::RussianRoulette{},
			&viewRadiance,
			0,// the path length is likely long already, do RR immediately
			viewPathThroughput))
		{
			lostEnergy += viewRadiance;
		}
	}

	return lostEnergy;
}

}// end namespace ph
