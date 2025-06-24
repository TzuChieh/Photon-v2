#include "Engine/Core/SurfaceBehavior/SurfaceOptics/IdealAbsorber.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"

#include <Common/assertion.h>

namespace ph
{

IdealAbsorber::IdealAbsorber() : 
	SurfaceOptics()
{
	m_phenomena.set(ESurfacePhenomenon::DiffuseReflection);
}

ESurfacePhenomenon IdealAbsorber::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DiffuseReflection;
}

void IdealAbsorber::calcBsdfCore(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	out.setContributability(false);
}

void IdealAbsorber::genBsdfSampleCore(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             /* sampleFlow */,
	BsdfSampleOutput&       out) const
{
	out.setContributability(false);
}

void IdealAbsorber::calcBsdfPdfCore(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdf({});
}

}// end namespace ph
