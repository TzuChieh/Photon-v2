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

	// Returns the validity of this sample. A valid sample means the sampling
	// process succeeded. Usable sample data can be expected.
	bool isValid() const;

	// For a sample to be good, it should not only be valid but also has a 
	// sample weight with finite value.
	bool isGood() const;

	void setValidity(bool isValid);

private:
	bool m_isValid{false};
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

inline bool BsdfSampleOutput::isValid() const
{
	return m_isValid;
}

inline bool BsdfSampleOutput::isGood() const
{
	return m_isValid && pdfAppliedBsdf.isFinite();
}

inline void BsdfSampleOutput::setValidity(const bool isValid)
{
	m_isValid = isValid;
}

}// end namespace ph