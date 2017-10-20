#pragma once

#include "Math/TVector3.h"
#include "Core/IntersectionDetail.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class BsdfEvaluation;

class BsdfSample final
{
public:
	class Input final
	{
	public:
		IntersectionDetail X;
		Vector3R           V;

		void set(const BsdfEvaluation& bsdfEval);

		inline void set(const IntersectionDetail& X, const Vector3R& V)
		{
			this->X = X;
			this->V = V;
		}
	};

	class Output final
	{
	public:
		Vector3R           L;
		SpectralStrength   pdfAppliedBsdf;
		ESurfacePhenomenon phenomenon;

		inline void set(const Vector3R& L, const ESurfacePhenomenon phenomenon)
		{
			this->L          = L;
			this->phenomenon = phenomenon;
		}

		inline bool isGood() const
		{
			return !pdfAppliedBsdf.isZero();
		}
	};

	Input  inputs;
	Output outputs;
};

}// end namespace ph