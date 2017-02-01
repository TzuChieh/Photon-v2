#pragma once

#include "Math/TVector3.h"

namespace ph
{

class Primitive;

class DirectLightSample final
{
public:
	Vector3R targetPos;
	Vector3R emitPos;
	Vector3R radianceLe;
	real pdfW;
	const Primitive* sourcePrim;

	inline DirectLightSample() : 
		targetPos(0, 0, 0), emitPos(0, 0, 0), radianceLe(0, 0, 0), pdfW(0.0f), sourcePrim(nullptr)
	{

	}

	inline bool isDirectSampleGood() const
	{
		return pdfW > 0.0_r && (radianceLe.x > 0.0_r || radianceLe.y > 0.0_r || radianceLe.z > 0.0_r);
	}

	inline void setDirectSample(const Vector3R& targetPos)
	{
		this->targetPos = targetPos;
	}
};

}// end namespace ph