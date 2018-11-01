#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"

namespace ph
{

struct Photon
{
	SpectralStrength radiance;
	SpectralStrength throughput;
	Vector3R position;
	Vector3R V;
};

}// end namespace ph