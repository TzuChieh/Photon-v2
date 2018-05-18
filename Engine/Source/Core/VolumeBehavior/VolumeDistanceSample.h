#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class VolumeDistanceSample final
{
public:
	class Input final
	{
	public:
		Vector3R X;
		Vector3R V;
		real maxDist;

		inline void set(const Vector3R& X, const Vector3R& V, const real maxDist)
		{
			this->X = X;
			this->V = V;
			this->maxDist = maxDist;
		}
	};

	class Output final
	{
	public:
		real             dist;
		SpectralStrength pdfAppliedWeight;
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