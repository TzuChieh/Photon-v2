#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"

namespace ph
{

IdealAbsorber::IdealAbsorber() : 
	SurfaceOptics()
{
	m_phenomena.set({});
}

void IdealAbsorber::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	out.bsdf.setValues(0.0_r);
}

void IdealAbsorber::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	out.pdfAppliedBsdf.setValues(0.0_r);
}

void IdealAbsorber::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph