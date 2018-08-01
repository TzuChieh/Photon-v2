#include "Core/SurfaceBehavior/SurfaceOptics/IdealTransmitter.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"

namespace ph
{

IdealTransmitter::IdealTransmitter() : 
	SurfaceOptics(),
	m_fresnel(std::make_shared<ExactDielectricFresnel>(1.0_r, 1.5_r))
{
	m_phenomena.set({ESP::SPECULAR_TRANSMISSION});
}

IdealTransmitter::~IdealTransmitter() = default;

void IdealTransmitter::evalBsdf(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	SpectralStrength* const   out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	out_bsdf->setValues(0.0_r);
}

void IdealTransmitter::genBsdfSample(
	const SurfaceHit&         X,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	Vector3R* const           out_L,
	SpectralStrength* const   out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	const Vector3R& N = X.getShadingNormal();
	Vector3R& L = *out_L;
	if(!m_fresnel->calcRefractDir(V, N, &L))
	{
		out_pdfAppliedBsdf->setValues(0.0_r);
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
	out_pdfAppliedBsdf->setValues(F.mul(iorRatio2 / cosI));
}

void IdealTransmitter::calcBsdfSamplePdf(
	const SurfaceHit&         X,
	const Vector3R&           L,
	const Vector3R&           V,
	const SidednessAgreement& sidedness,
	real* const               out_pdfW) const
{
	PH_ASSERT(out_pdfW);

	*out_pdfW = 0.0_r;
}

}// end namespace ph