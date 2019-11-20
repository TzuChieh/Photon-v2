#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

#include <array>
#include <utility>

namespace ph
{

class BsdfEvaluation;

class BsdfSampleInput final
{
public:
	using Sample = std::array<real, 2>;

	//Sample           samples;
	SurfaceHit       X;
	math::Vector3R   V;
	SurfaceElemental elemental;
	ETransport       transported;

	void set(/*Sample samples, */const BsdfEvaluation& eval);

	void set(
		//Sample                 samples,
		const SurfaceHit&      X, 
		const math::Vector3R&  V);

	void set(
		//Sample                 samples,
		const SurfaceHit&      X, 
		const math::Vector3R&  V,
		const SurfaceElemental elemental,
		const ETransport       transported); 
};

class BsdfSampleOutput final
{
public:
	math::Vector3R   L;
	SpectralStrength pdfAppliedBsdf;

	// Tells whether this sample has non-zero and sane contribution. All 
	// sample data should be usable if true is returned; otherwise, zero
	// contribution is implied, and sample data is undefined.
	bool isMeasurable() const;

	void setMeasurability(bool measurability);

private:
	bool m_isMeasurable{false};
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
	//Sample                 samples,
	const SurfaceHit&      X, 
	const math::Vector3R&  V)
{
	set(
		//std::move(samples), 
		X, 
		V, 
		ALL_ELEMENTALS, 
		ETransport::RADIANCE);
}

inline void BsdfSampleInput::set(
	//Sample                 samples,
	const SurfaceHit&      X, 
	const math::Vector3R&  V,
	const SurfaceElemental elemental,
	const ETransport       transported)
{
	//this->samples     = std::move(samples);
	this->X           = X;
	this->V           = V;
	this->elemental   = elemental;
	this->transported = transported;
}

inline bool BsdfSampleOutput::isMeasurable() const
{
	return m_isMeasurable;
}

inline void BsdfSampleOutput::setMeasurability(const bool measurability)
{
	m_isMeasurable = measurability;
}

}// end namespace ph
