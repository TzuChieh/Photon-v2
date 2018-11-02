#pragma once

#include "Core/Intersectable/IndexedKdtree/TCenterKdtree.h"
#include "Core/Renderer/PPM/Photon.h"
#include "Math/TVector3.h"

namespace ph
{

struct PhotonCenterCalculator
{
	Vector3R operator () (const Photon& photon) const
	{
		return photon.position;
	}
};

using PhotonMap = TCenterKdtree<Photon, int, PhotonCenterCalculator>;

}// end namespace ph