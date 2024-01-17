#pragma once

#include "Math/Algorithm/IndexedKdtree/TIndexedPointKdtree.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Math/TVector3.h"

#include <Common/primitive_type.h>

#include <type_traits>

namespace ph
{

/*! @brief Default photon map type. Should be adequate for most cases.
*/
template<CPhoton Photon>
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

	using MapType = math::TIndexedPointKdtree<Photon, int, PhotonCenterCalculator>;

	MapType map = MapType(2, PhotonCenterCalculator{});

	/*! Minimum length of photon paths in this photon map. If a photon lands on a surface for the
	first time, then its path length is 1 (no need to bounce).
	*/
	uint32 minPhotonPathLength = 1;

	/*! Maximum length of photon paths in this photon map. The default value is practically
	infinite number of bounces.
	*/
	uint32 maxPhotonPathLength = 16384;
};

}// end namespace ph
