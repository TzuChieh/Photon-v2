#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

namespace ph
{

class BsdfEvaluation;

class BsdfSample final
{
public:
	class Input final
	{
	public:
		SurfaceHit       X;
		Vector3R         V;
		SurfaceElemental elemental;

		void set(const BsdfEvaluation& bsdfEval);

		inline void set(
			const SurfaceHit&      X, 
			const Vector3R&        V)
		{
			set(X, V, ALL_ELEMENTALS);
		}

		inline void set(
			const SurfaceHit&      X, 
			const Vector3R&        V,
			const SurfaceElemental elemental)
		{
			this->X         = X;
			this->V         = V;
			this->elemental = elemental;
		}
	};

	class Output final
	{
	public:
		Vector3R         L;
		SpectralStrength pdfAppliedBsdf;
		SurfaceElemental elemental;

		inline bool isGood() const
		{
			return !pdfAppliedBsdf.isZero();
		}
	};

	Input  inputs;
	Output outputs;
};

}// end namespace ph