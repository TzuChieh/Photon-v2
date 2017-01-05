#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Intersection;

enum class ESurfaceSampleType : int32
{
	REFLECTION,
	TRANSMISSION, 
	DELTA, 
	UNSPECIFIED
};

class SurfaceSample final
{
public:
	const Intersection* X;
	Vector3f            L;
	Vector3f            V;
	Vector3f            liWeight;
	ESurfaceSampleType  type;

	inline SurfaceSample() : 
		X(nullptr), L(0, 0, -1), V(0, 0, -1), liWeight(0, 0, 0), type(ESurfaceSampleType::UNSPECIFIED)
	{

	}

	inline bool isImportanceSampleGood() const
	{
		return liWeight.x > 0.0f || liWeight.y > 0.0f || liWeight.z > 0.0f;
	}

	inline bool isEvaluationGood() const
	{
		return liWeight.x > 0.0f || liWeight.y > 0.0f || liWeight.z > 0.0f;
	}

	inline void setImportanceSample(const Intersection& X, const Vector3f& V)
	{
		this->X = &X;
		this->V = V;
	}

	inline void setEvaluation(const Intersection& X, const Vector3f& L, const Vector3f& V)
	{
		this->X = &X;
		this->L = L;
		this->V = V;
	}
};

}// end namespace ph