#pragma once

#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"

namespace ph
{

class BsdfEvaluation;
class BsdfSample;

class BsdfPdfQuery final
{
public:
	class Input final
	{
	public:
		Intersection       X;
		Vector3R           L;
		Vector3R           V;
		ESurfacePhenomenon phenomenon;

		void set(const BsdfEvaluation& bsdfEval);
		void set(const BsdfSample& sample);

		inline void set(const Intersection& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon phenomenon)
		{
			this->X          = X;
			this->L          = L;
			this->V          = V;
			this->phenomenon = phenomenon;
		}
	};

	class Output final
	{
	public:
		real sampleDirPdfW;

		inline void set(const real sampleDirPdfW)
		{
			this->sampleDirPdfW = sampleDirPdfW;
		}
	};

	Input  inputs;
	Output outputs;
};

}// end namespace ph