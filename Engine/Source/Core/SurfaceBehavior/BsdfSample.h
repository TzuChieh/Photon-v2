#pragma once

#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"

namespace ph
{

class BsdfEvaluation;

class BsdfSample final
{
public:
	class Input final
	{
	public:
		Intersection X;
		Vector3R     V;

		void set(const BsdfEvaluation& bsdfEval);

		inline void set(const Intersection& X, const Vector3R& V)
		{
			this->X = X;
			this->V = V;
		}
	};

	class Output final
	{
	public:
		Vector3R           L;
		Vector3R           pdfAppliedBsdf;
		ESurfacePhenomenon phenomenon;

		inline void set(const Vector3R& L, const ESurfacePhenomenon phenomenon)
		{
			this->L          = L;
			this->phenomenon = phenomenon;
		}

		inline bool isGood() const
		{
			return pdfAppliedBsdf.x >= 0.0_r && pdfAppliedBsdf.y >= 0.0_r && pdfAppliedBsdf.z >= 0.0_r;
		}
	};

	Input  inputs;
	Output outputs;
};

}// end namespace ph