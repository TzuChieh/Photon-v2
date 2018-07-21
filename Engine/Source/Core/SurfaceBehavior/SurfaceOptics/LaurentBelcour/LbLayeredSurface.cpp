#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableFGD.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableTIR.h"

namespace ph
{

LbLayeredSurface::LbLayeredSurface() : 
	SurfaceOptics()
{
	TableTIR tir;
	tir.loadFromFile(Path(""));
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

}// end namespace ph