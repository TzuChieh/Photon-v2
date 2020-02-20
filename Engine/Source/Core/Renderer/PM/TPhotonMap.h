#pragma once

#include "Math/Algorithm/IndexedKdtree/TIndexedPointKdtree.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Math/TVector3.h"

#include <type_traits>

namespace ph
{

template<typename Photon>
struct TPhotonCenterCalculator
{
	static_assert(std::is_base_of_v<TPhoton<Photon>, Photon>);

	math::Vector3R operator () (const Photon& photon) const
	{
		static_assert(Photon::template has<EPhotonData::POSITION>());

		return photon.template get<EPhotonData::POSITION>();
	}
};

template<typename Photon>
using TPhotonMap = math::TIndexedPointKdtree<Photon, int, TPhotonCenterCalculator<Photon>>;

}// end namespace ph
