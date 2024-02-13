#pragma once

#include "Math/Algorithm/IndexedKdtree/TIndexedPointKdtree.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Renderer/PM/PMCommonParams.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <cstddef>
#include <type_traits>
#include <vector>

namespace ph
{

/*! @brief Carries common informatiom for a photon map.
@see `TPhotonMap` for what each parameter means.
*/
template<CPhoton Photon>
class TPhotonMapInfo final
{
public:
	std::size_t numPaths = 0;
	uint32 minPathLength = 1;
	uint32 maxPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH;
};

/*! @brief Default photon map type. Should be adequate for most cases.
*/
template
<
	CPhoton Photon, 
	math::CIndexedPointKdtreeItemStorage<Photon> PhotonStorage = std::vector<Photon>
>
class TPhotonMap final
{
public:
	struct PhotonCenterCalculator
	{
		static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

		math::Vector3R operator () (const Photon& photon) const
		{
			static_assert(Photon::template has<EPhotonData::Position>());

			return photon.template get<EPhotonData::Position>();
		}
	};

	using MapType = math::TIndexedPointKdtree<Photon, int, PhotonCenterCalculator, PhotonStorage>;
	using BuildCacheType = MapType::BuildCache;

	MapType map = MapType(2, PhotonCenterCalculator{});

	/*! Number of photon paths in this photon map.
	*/
	std::size_t numPaths = 0;

	/*! Minimum length of photon paths in this photon map. If a photon lands on a surface for the
	first time, then its path length is 1 (no need to bounce).
	*/
	uint32 minPathLength = 1;

	/*! Maximum length of photon paths in this photon map. The default value is practically
	infinite number of bounces.
	*/
	uint32 maxPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH;

	/*!
	@param viewPathLength The view path length used for forming a full path.
	@param minFullPathLength The minimum full path length to form.
	@param maxFullPathLength The maximum full path length to form (inclusive).
	@return Can this photon map form any paths in the specified full path length range. 
	*/
	bool canContribute(
		const std::size_t viewPathLength,
		const std::size_t minFullPathLength,
		const std::size_t maxFullPathLength) const
	{
		PH_ASSERT_LE(minFullPathLength, maxFullPathLength);
		const auto minFullPathLengthFromMap = viewPathLength + minPathLength;
		const auto maxFullPathLengthFromMap = viewPathLength + maxPathLength;
		return maxFullPathLength >= minFullPathLengthFromMap && 
		       minFullPathLength <= maxFullPathLengthFromMap;
	}

	/*! @brief Find all photons in a radius.
	*/
	void find(
		const math::Vector3R& position,
		const real            kernelRadius,
		std::vector<Photon>&  photons) const
	{
		map.findWithinRange(position, kernelRadius, photons);
	}

	/*! @brief Find all photons in a radius that can contribute given the path requirements.
	@see `canContribute()`.
	*/
	void find(
		const math::Vector3R& position,
		const real            kernelRadius,
		const std::size_t     viewPathLength,
		const std::size_t     minFullPathLength,
		const std::size_t     maxFullPathLength,
		std::vector<Photon>&  photons) const
	{
		if(!canContribute(viewPathLength, minFullPathLength, maxFullPathLength))
		{
			return;
		}

		if constexpr(Photon::template has<EPhotonData::PathLength>())
		{
			const auto kernelRadius2 = kernelRadius * kernelRadius;

			map.rangeTraversal(
				position,
				kernelRadius2,
				[
					this, kernelRadius2, viewPathLength, minFullPathLength, maxFullPathLength, 
					&position, &photons
				]
				(const Photon& photon)
				{
					const auto pos         = PhotonCenterCalculator{}(photon);
					const auto dist2       = (pos - position).lengthSquared();
					const auto fullPathLen = viewPathLength + photon.template get<EPhotonData::PathLength>();
					if(dist2 < kernelRadius2 && 
					   minFullPathLength <= fullPathLen && fullPathLen <= maxFullPathLength)
					{
						photons.push_back(photon);
					}
				});
		}
		else
		{
			// No path length available, this is an error.
			PH_DEFAULT_LOG(ErrorOnce, 
				"The photon type in this photon map contains no path length info. Cannot find "
				"photon for the specified full path length in [{}, {}]. This photon map "
				"contains path length in [{}, {}].", 
				minFullPathLength, maxFullPathLength, minPathLength, maxPathLength);
		}
	}

	TPhotonMapInfo<Photon> getInfo() const
	{
		return {
			.numPaths = numPaths,
			.minPathLength = minPathLength,
			.maxPathLength = maxPathLength};
	}
};

}// end namespace ph
