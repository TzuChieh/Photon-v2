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
		ETransport       transported;

		void set(const BsdfEvaluation& bsdfEval);

		inline void set(
			const SurfaceHit&      X, 
			const Vector3R&        V)
		{
			set(X, V, ALL_ELEMENTALS, ETransport::RADIANCE);
		}

		inline void set(
			const SurfaceHit&      X, 
			const Vector3R&        V,
			const SurfaceElemental elemental,
			const ETransport       transported)
		{
			this->X           = X;
			this->V           = V;
			this->elemental   = elemental;
			this->transported = transported;
		}
	};

	class Output final
	{
	public:
		Vector3R         L;
		SpectralStrength pdfAppliedBsdf;

		inline bool isGood() const
		{
			return !pdfAppliedBsdf.isZero();
		}
	};

	Input  inputs;
	Output outputs;
};

}// end namespace ph