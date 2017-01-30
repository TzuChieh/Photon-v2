#pragma once

#include "Math/TVector3.h"

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
	Vector3R            L;
	Vector3R            V;
	Vector3R            liWeight;
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

	inline void setImportanceSample(const Intersection& X, const Vector3R& V)
	{
		this->X = &X;
		this->V = V;
	}

	inline void setEvaluation(const Intersection& X, const Vector3R& L, const Vector3R& V)
	{
		this->X = &X;
		this->L = L;
		this->V = V;
	}
};

}// end namespace ph