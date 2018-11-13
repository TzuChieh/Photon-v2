#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

namespace ph
{

class BsdfEvaluation;

class BsdfSampleInput final
{
public:
	SurfaceHit       X;
	Vector3R         V;
	SurfaceElemental elemental;
	ETransport       transported;

	void set(const BsdfEvaluation& eval);

	void set(
		const SurfaceHit&      X, 
		const Vector3R&        V);

	void set(
		const SurfaceHit&      X, 
		const Vector3R&        V,
		const SurfaceElemental elemental,
		const ETransport       transported);
};

class BsdfSampleOutput final
{
public:
	Vector3R         L;
	SpectralStrength pdfAppliedBsdf;

	bool isGood() const;
};

class BsdfSample final
{
public:
	using Input  = BsdfSampleInput;
	using Output = BsdfSampleOutput;

	Input  inputs;
	Output outputs;
};

// In-header Implementations:

inline void BsdfSampleInput::set(
	const SurfaceHit&      X, 
	const Vector3R&        V)
{
	set(X, V, ALL_ELEMENTALS, ETransport::RADIANCE);
}

inline void BsdfSampleInput::set(
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

inline bool BsdfSampleOutput::isGood() const
{
	return pdfAppliedBsdf.isFinite();
}

}// end namespace ph