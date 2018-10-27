#pragma once

#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

namespace ph
{

class BsdfEvaluation;
class BsdfSample;
class BsdfSampleInput;
class BsdfSampleOutput;

class BsdfPdfQueryInput final
{
public:
	SurfaceHit       X;
	Vector3R         L;
	Vector3R         V;
	SurfaceElemental elemental;

	void set(const BsdfEvaluation& bsdfEval);
	void set(const BsdfSample& sample);
	void set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput);

	void set(
		const SurfaceHit&      X, 
		const Vector3R&        L,
		const Vector3R&        V,
		const SurfaceElemental elemental);
};

class BsdfPdfQueryOutput final
{
public:
	real sampleDirPdfW;
};

class BsdfPdfQuery final
{
public:
	using Input  = BsdfPdfQueryInput;
	using Output = BsdfPdfQueryOutput;

	Input  inputs;
	Output outputs;
};

// In-header Implementations:

inline void BsdfPdfQueryInput::set(
	const SurfaceHit&      X,
	const Vector3R&        L,
	const Vector3R&        V,
	const SurfaceElemental elemental)
{
	this->X = X;
	this->L = L;
	this->V = V;
	this->elemental = elemental;
}

}// end namespace ph