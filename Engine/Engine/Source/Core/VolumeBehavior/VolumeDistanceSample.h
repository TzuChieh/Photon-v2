#pragma once

#include "Math/TVector3.h"
#include "Math/Color/Spectrum.h"
#include "Core/SurfaceHit.h"

#include <Common/primitive_type.h>

namespace ph
{

class VolumeDistanceSample final
{
public:
	class Input final
	{
	public:
		SurfaceHit X;
		math::Vector3R L;
		real maxDist;

		inline void set(const SurfaceHit& X, const math::Vector3R& L, const real maxDist)
		{
			this->X = X;
			this->L = L;
			this->maxDist = maxDist;
		}
	};

	class Output final
	{
	public:
		real dist;
		math::Spectrum pdfAppliedWeight;
	};

	inline bool isMaxDistReached() const
	{
		return outputs.dist >= inputs.maxDist;
	}

public:
	Input  inputs;
	Output outputs;
};

}// end namespace ph
