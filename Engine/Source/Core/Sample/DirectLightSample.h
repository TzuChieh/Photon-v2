#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Primitive;

class DirectLightSample final
{
public:
	Vector3f targetPos;
	Vector3f emitPos;
	Vector3f radianceLe;
	float32 pdfW;
	const Primitive* sourcePrim;

	inline DirectLightSample() : 
		targetPos(0, 0, 0), emitPos(0, 0, 0), radianceLe(0, 0, 0), pdfW(0.0f), sourcePrim(nullptr)
	{

	}

	inline bool isDirectSampleGood() const
	{
		return pdfW > 0.0f && (radianceLe.x > 0.0f || radianceLe.y > 0.0f || radianceLe.z > 0.0f);
	}

	inline void setDirectSample(const Vector3f& targetPos)
	{
		this->targetPos = targetPos;
	}
};

}// end namespace ph