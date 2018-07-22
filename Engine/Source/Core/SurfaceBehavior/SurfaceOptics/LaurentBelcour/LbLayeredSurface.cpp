#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

LbLayeredSurface::LbLayeredSurface() : 
	SurfaceOptics()
{
	const TableFGD& fgd = FGD();
	const TableTIR& tir = TIR();
}

LbLayeredSurface::~LbLayeredSurface() = default;

void LbLayeredSurface::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* out_bsdf) const
{
	// TODO
}

void LbLayeredSurface::genBsdfSample(
	const SurfaceHit& X, const Vector3R& V,
	Vector3R* out_L,
	SpectralStrength* out_pdfAppliedBsdf) const
{
	// TODO
}

void LbLayeredSurface::calcBsdfSamplePdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	real* out_pdfW) const
{
	// TODO
}

real LbLayeredSurface::alphaToVariance(const real alpha)
{
	PH_ASSERT(alpha > 0.0_r);

	const real alpha1p1 = std::pow(alpha, 1.1_r);
	return alpha1p1 / (1.0_r - alpha1p1);
}

real LbLayeredSurface::varianceToAlpha(const real variance)
{
	return std::pow(variance / (1.0_r + variance), 1.0_r / 1.1_r);
}

real LbLayeredSurface::gToVariance(const real g)
{
	PH_ASSERT(0.0_r < g && g <= 1.0_r);

	return std::pow((1.0_r - g) / g, 0.8_r) / (1.0_r + g);
}

}// end namespace ph