#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

namespace ph
{

class BsdfSample;
class BsdfSampleInput;
class BsdfSampleOutput;

class BsdfEvaluationInput final
{
public:
	SurfaceHit       X;
	Vector3R         L;
	Vector3R         V;
	SurfaceElemental elemental;
	ETransport       transported;

	void set(const BsdfSample& sample);
	void set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput);

	void set(
		const SurfaceHit&      X, 
		const Vector3R&        L, 
		const Vector3R&        V);

	void set(
		const SurfaceHit&      X,
		const Vector3R&        L,
		const Vector3R&        V,
		const SurfaceElemental elemental);

	void set(
		const SurfaceHit&      X,
		const Vector3R&        L,
		const Vector3R&        V,
		const ETransport       transported);

	void set(
		const SurfaceHit&      X, 
		const Vector3R&        L, 
		const Vector3R&        V, 
		const SurfaceElemental elemental,
		const ETransport       transported);
};

class BsdfEvaluationOutput
{
public:
	SpectralStrength bsdf;

	bool isGood() const;
};

class BsdfEvaluation final
{
public:
	using Input  = BsdfEvaluationInput;
	using Output = BsdfEvaluationOutput;
	
	Input  inputs;
	Output outputs;
};

// In-header Implementations:

inline void BsdfEvaluationInput::set(
	const SurfaceHit&      X, 
	const Vector3R&        L, 
	const Vector3R&        V)
{
	set(X, L, V, ALL_ELEMENTALS, ETransport::RADIANCE);
}

inline void BsdfEvaluationInput::set(
	const SurfaceHit&      X,
	const Vector3R&        L,
	const Vector3R&        V,
	const SurfaceElemental elemental)
{
	set(X, L, V, elemental, ETransport::RADIANCE);
}

inline void BsdfEvaluationInput::set(
	const SurfaceHit&      X,
	const Vector3R&        L,
	const Vector3R&        V,
	const ETransport       transported)
{
	set(X, L, V, ALL_ELEMENTALS, transported);
}

inline void BsdfEvaluationInput::set(
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

inline bool BsdfEvaluationOutput::isGood() const
{
	return !bsdf.isZero();
}

}// end namespace ph