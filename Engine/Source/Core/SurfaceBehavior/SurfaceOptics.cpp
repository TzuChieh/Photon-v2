#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SidednessAgreement.h"

namespace ph
{

SurfaceOptics::SurfaceOptics() : 
	m_phenomena()
{}

SurfaceOptics::~SurfaceOptics() = default;

void SurfaceOptics::evalBsdf(BsdfEvaluation& eval) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	evalBsdf(
		eval.inputs.X, 
		eval.inputs.L, 
		eval.inputs.V, 
		sidedness,
		&(eval.outputs.bsdf));
}

void SurfaceOptics::genBsdfSample(BsdfSample& sample) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	genBsdfSample(
		sample.inputs.X, 
		sample.inputs.V,
		sidedness,
		&(sample.outputs.L), &(sample.outputs.pdfAppliedBsdf));
}

void SurfaceOptics::calcBsdfSamplePdf(BsdfPdfQuery& query) const
{
	const SidednessAgreement sidedness(ESaPolicy::STRICT);

	calcBsdfSamplePdf(
		query.inputs.X, 
		query.inputs.L, 
		query.inputs.V,
		sidedness,
		&(query.outputs.sampleDirPdfW));
}

}// end namespace ph