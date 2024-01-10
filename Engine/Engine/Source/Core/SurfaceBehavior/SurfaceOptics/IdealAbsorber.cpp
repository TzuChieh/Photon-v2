#include "Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"

#include <Common/assertion.h>

namespace ph
{

IdealAbsorber::IdealAbsorber() : 
	SurfaceOptics()
{
	m_phenomena.set({ESurfacePhenomenon::DiffuseReflection});
}

ESurfacePhenomenon IdealAbsorber::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DiffuseReflection;
}

void IdealAbsorber::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setMeasurability(false);
}

void IdealAbsorber::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             /* sampleFlow */,
	BsdfSampleOutput&       out) const
{
	out.setMeasurability(false);
}

void IdealAbsorber::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.sampleDirPdfW = 0.0_r;
}

}// end namespace ph
