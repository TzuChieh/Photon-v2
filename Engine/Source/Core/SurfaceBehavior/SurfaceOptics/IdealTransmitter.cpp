#include "Core/SurfaceBehavior/SurfaceOptics/IdealTransmitter.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/assertion.h"

namespace ph
{

IdealTransmitter::IdealTransmitter(const std::shared_ptr<DielectricFresnel>& fresnel) :
	SurfaceOptics(),
	m_fresnel(fresnel)
{
	PH_ASSERT(fresnel);

	m_phenomena.set({ESP::DELTA_TRANSMISSION});
}

void IdealTransmitter::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	out.bsdf.setValues(0.0_r);
}

void IdealTransmitter::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	const Vector3R& N = in.X.getShadingNormal();
	Vector3R& L = out.L;
	if(!m_fresnel->calcRefractDir(in.V, N, &L))
	{
		out.pdfAppliedBsdf.setValues(0.0_r);
		return;
	}

	real cosI = N.dot(L);
	SpectralStrength F;
	m_fresnel->calcTransmittance(cosI, &F);

	real transportFactor = 1.0_r;
	if(in.transported == ETransport::RADIANCE)
	{
		real etaI = m_fresnel->getIorOuter();
		real etaT = m_fresnel->getIorInner();
		if(cosI < 0.0_r)
		{
			std::swap(etaI, etaT);
		}

		transportFactor = (etaT * etaT) / (etaI * etaI);
	}
	
	out.pdfAppliedBsdf.setValues(F.mul(transportFactor / std::abs(cosI)));
}

void IdealTransmitter::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph