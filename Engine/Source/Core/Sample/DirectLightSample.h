#pragma once

#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class Primitive;

class DirectLightSample final
{
public:
	Vector3R         targetPos;
	Vector3R         emitPos;
	SpectralStrength radianceLe;
	real pdfW;
	const Primitive* sourcePrim;

	inline DirectLightSample() : 
		targetPos(0, 0, 0), emitPos(0, 0, 0), radianceLe(0), pdfW(0), sourcePrim(nullptr)
	{}

	inline bool isDirectSampleGood() const
	{
		return pdfW > 0.0_r;
	}

	inline void setDirectSample(const Vector3R& targetPos)
	{
		this->targetPos = targetPos;
	}
};

}// end namespace ph