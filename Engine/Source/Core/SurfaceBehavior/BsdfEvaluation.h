#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

namespace ph
{

class BsdfEvaluation final
{
public:
	class Input final
	{
	public:
		SurfaceHit       X;
		Vector3R         L;
		Vector3R         V;
		SurfaceElemental elemental;
		ETransport       transported;

		inline void set(
			const SurfaceHit&      X, 
			const Vector3R&        L, 
			const Vector3R&        V)
		{
			set(X, L, V, ALL_ELEMENTALS, ETransport::RADIANCE);
		}

		inline void set(
			const SurfaceHit&      X,
			const Vector3R&        L,
			const Vector3R&        V,
			const SurfaceElemental elemental)
		{
			set(X, L, V, elemental, ETransport::RADIANCE);
		}

		inline void set(
			const SurfaceHit&      X,
			const Vector3R&        L,
			const Vector3R&        V,
			const ETransport       transported)
		{
			set(X, L, V, ALL_ELEMENTALS, transported);
		}

		inline void set(
			const SurfaceHit&      X, 
			const Vector3R&        L, 
			const Vector3R&        V, 
			const SurfaceElemental elemental,
			const ETransport       transported)
		{
			this->X           = X;
			this->L           = L;
			this->V           = V;
			this->elemental   = elemental;
			this->transported = transported;
		}
	};

	class Output final
	{
	public:
		SpectralStrength bsdf;

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