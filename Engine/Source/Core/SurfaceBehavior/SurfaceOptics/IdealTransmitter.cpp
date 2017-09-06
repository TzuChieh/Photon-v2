#include "Core/SurfaceBehavior/SurfaceOptics/IdealTransmitter.h"
#include "Core/SurfaceBehavior/Utility/ExactDielectricFresnel.h"

namespace ph
{

IdealTransmitter::IdealTransmitter() : 
	SurfaceOptics(),
	m_fresnel(std::make_shared<ExactDielectricFresnel>(1.0_r, 1.5_r))
{

}

IdealTransmitter::~IdealTransmitter() = default;

void IdealTransmitter::evalBsdf(
	const Intersection& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf,
	ESurfacePhenomenon* const out_type) const
{
	out_bsdf->set(0.0_r);
	*out_type = ESurfacePhenomenon::TRANSMISSION;
}

void IdealTransmitter::genBsdfSample(
	const Intersection& X, const Vector3R& V,
	Vector3R* const out_L,
	SpectralStrength* const out_pdfAppliedBsdf,
	ESurfacePhenomenon* const out_type) const
{
	*out_type = ESurfacePhenomenon::TRANSMISSION;

	const Vector3R& N = X.getHitSmoothNormal();
	Vector3R& L = *out_L;
	if(!m_fresnel->calcRefractDir(V, N, &L))
	{
		out_pdfAppliedBsdf->set(0.0_r);
		return;
	}

	real cosI = N.dot(L);
	SpectralStrength F;
	m_fresnel->calcTransmittance(cosI, &F);

	real etaI = m_fresnel->getIorOuter();
	real etaT = m_fresnel->getIorInner();
	if(cosI < 0.0_r)
	{
		std::swap(etaI, etaT);
		cosI = std::abs(cosI);
	}

	const real iorRatio2 = (etaT * etaT) / (etaI * etaI);
	out_pdfAppliedBsdf->set(F.mul(iorRatio2 / cosI));
}

void IdealTransmitter::calcBsdfSamplePdf(
	const Intersection& X, const Vector3R& L, const Vector3R& V,
	const ESurfacePhenomenon& type,
	real* const out_pdfW) const
{
	*out_pdfW = 0.0_r;
}

}// end namespace ph