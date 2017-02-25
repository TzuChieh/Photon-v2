#pragma once

#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"

namespace ph
{

class BsdfEvaluation final
{
public:
	class Input final
	{
	public:
		Intersection X;
		Vector3R     L;
		Vector3R     V;

		inline void set(const Intersection& X, const Vector3R& L, const Vector3R& V)
		{
			this->X = X;
			this->L = L;
			this->V = V;
		}
	};

	class Output final
	{
	public:
		Vector3R           bsdf;
		ESurfacePhenomenon phenomenon;

		inline void set(const Vector3R& bsdf, const ESurfacePhenomenon phenomenon)
		{
			this->bsdf       = bsdf;
			this->phenomenon = phenomenon;
		}

		inline bool isGood() const
		{
			return bsdf.x >= 0.0_r && bsdf.y >= 0.0_r && bsdf.z >= 0.0_r;
		}
	};

	typedef Input  InputType;
	typedef Output OutputType;

	InputType  inputs;
	OutputType outputs;
};

}// end namespace ph