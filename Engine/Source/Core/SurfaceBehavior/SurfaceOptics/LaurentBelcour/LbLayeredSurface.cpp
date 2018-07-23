#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/InterfaceStatistics.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayer.h"

#include <cmath>

namespace ph
{

LbLayeredSurface::LbLayeredSurface(
	const std::vector<SpectralStrength>& iorNs,
	const std::vector<SpectralStrength>& iorKs,
	const std::vector<real>&             alphas) :

	SurfaceOptics(),

	m_iorNs(iorNs), m_iorKs(iorKs), m_alphas(alphas)
{
	PH_ASSERT(m_iorNs.size() != 0);
	PH_ASSERT(m_iorNs.size() == m_iorKs.size() && m_iorKs.size() == m_alphas.size());

	// DEBUG
	InterfaceStatistics test(1.0_r, LbLayer(SpectralStrength(1.0_r)));
}

LbLayeredSurface::~LbLayeredSurface() = default;

void LbLayeredSurface::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	out_bsdf->setValues(0);

	Vector3R H;
	if(!BsdfHelper::makeHalfVector(L, V, &H))
	{
		return;
	}

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