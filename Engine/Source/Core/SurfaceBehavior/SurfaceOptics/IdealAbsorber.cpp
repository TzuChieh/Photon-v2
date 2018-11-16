#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Common/assertion.h"

namespace ph
{

IdealAbsorber::IdealAbsorber() : 
	SurfaceOptics()
{
	m_phenomena.set({ESurfacePhenomenon::DIFFUSE_REFLECTION});
}

ESurfacePhenomenon IdealAbsorber::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DIFFUSE_REFLECTION;
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