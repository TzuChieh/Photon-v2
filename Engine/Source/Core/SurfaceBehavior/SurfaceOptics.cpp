#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SidednessAgreement.h"

namespace ph
{

SurfaceOptics::SurfaceOptics() : 
	m_phenomena(),
	m_numComponents(1)
{}

SurfaceOptics::~SurfaceOptics() = default;

void SurfaceOptics::calcBsdf(BsdfEvaluation& eval) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	calcBsdf(
		eval.inputs.X, 
		eval.inputs.L, 
		eval.inputs.V, 
		sidedness,
		&(eval.outputs.bsdf));
}

void SurfaceOptics::calcBsdfSample(BsdfSample& sample) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	calcBsdfSample(
		sample.inputs.X, 
		sample.inputs.V,
		sidedness,
		&(sample.outputs.L), &(sample.outputs.pdfAppliedBsdf));
}

void SurfaceOptics::calcBsdfSamplePdfW(BsdfPdfQuery& query) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	calcBsdfSamplePdfW(
		query.inputs.X, 
		query.inputs.L, 
		query.inputs.V,
		sidedness,
		&(query.outputs.sampleDirPdfW));
}

}// end namespace ph