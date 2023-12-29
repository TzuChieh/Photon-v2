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
	uint32 minPhotonBounces = 1;
	uint32 maxPhotonBounces = 16384;
};

}// end namespace ph
