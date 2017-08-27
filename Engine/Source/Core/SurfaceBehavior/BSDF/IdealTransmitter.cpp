#include "Core/SurfaceBehavior/BSDF/IdealTransmitter.h"

namespace ph
{

IdealTransmitter::~IdealTransmitter() = default;


void IdealTransmitter::evaluate(
	const Intersection& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf,
	ESurfacePhenomenon* const out_type) const
{
	// TODO
}

void IdealTransmitter::genSample(
	const Intersection& X, const Vector3R& V,
	Vector3R* const out_L,
	SpectralStrength* const out_pdfAppliedBsdf,
	ESurfacePhenomenon* const out_type) const
{
	// TODO
}

void IdealTransmitter::calcSampleDirPdfW(
	const Intersection& X, const Vector3R& L, const Vector3R& V,
	const ESurfacePhenomenon& type,
	real* const out_pdfW) const
{
	// TODO
}

}// end namespace ph