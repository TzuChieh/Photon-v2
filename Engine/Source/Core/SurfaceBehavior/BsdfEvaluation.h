#pragma once

#include "Math/TVector3.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class BsdfEvaluation final
{
public:
	class Input final
	{
	public:
		HitDetail X;
		Vector3R  L;
		Vector3R  V;

		inline void set(const HitDetail& X, const Vector3R& L, const Vector3R& V)
		{
			this->X = X;
			this->L = L;
			this->V = V;
		}
	};

	class Output final
	{
	public:
		SpectralStrength   bsdf;
		ESurfacePhenomenon phenomenon;

		inline void set(const SpectralStrength& bsdf, const ESurfacePhenomenon phenomenon)
		{
			this->bsdf       = bsdf;
			this->phenomenon = phenomenon;
		}

		inline bool isGood() const
		{
			return !bsdf.isZero();
		}
	};

	typedef Input  InputType;
	typedef Output OutputType;

	InputType  inputs;
	OutputType outputs;
};

}// end namespace ph