#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"

namespace ph
{

SurfaceOptics::SurfaceOptics() : 
	m_phenomena(),
	m_numElementals(1)
{}

SurfaceOptics::~SurfaceOptics() = default;

void SurfaceOptics::calcBsdf(BsdfEvaluation& eval) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	calcBsdf(
		eval.inputs,
		eval.outputs,
		sidedness);
}

void SurfaceOptics::calcBsdfSample(BsdfSample& sample) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	calcBsdfSample(
		sample.inputs, 
		sample.outputs,
		sidedness);
}

void SurfaceOptics::calcBsdfSamplePdfW(BsdfPdfQuery& query) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	calcBsdfSamplePdfW(
		query.inputs,
		query.outputs,
		sidedness);
}

}// end namespace ph
