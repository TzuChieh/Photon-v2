#pragma once

#include "Common/primitive_type.h"
#include "Core/SurfaceHit.h"
#include "Math/TVector2.h"
#include "Core/Quantity/SpectralStrength.h"

#include <cstddef>

namespace ph
{

struct Viewpoint
{
	SurfaceHit hit;
	Vector2R filmNdcPos;
	real radius;
	std::size_t numPhotons;
	SpectralStrength throughput;
	Vector3R L;
};

}// end namespace ph