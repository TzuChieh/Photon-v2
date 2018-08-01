#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Estimator/Utility/TSidednessAgreement.h"

namespace ph
{

SurfaceOptics::SurfaceOptics() : 
	m_phenomena()
{}

SurfaceOptics::~SurfaceOptics() = default;

void SurfaceOptics::evalBsdf(BsdfEvaluation& eval) const
{
	const TSidednessAgreement<ESaPolicy::STRICT> sidedness;
	const bool isSameHemisphere = sidedness.isSameHemisphere(eval.inputs.X, eval.inputs.V, eval.inputs.L);
	const bool isOppositeHemisphere = sidedness.isOppositeHemisphere(eval.inputs.X, eval.inputs.V, eval.inputs.L);

	if((isSameHemisphere && m_phenomena.hasAtLeastOne({ESP::DIFFUSE_REFLECTION, ESP::GLOSSY_REFLECTION, ESP::SPECULAR_REFLECTION})) ||
	   (isOppositeHemisphere && m_phenomena.hasAtLeastOne({ESP::GLOSSY_TRANSMISSION, ESP::SPECULAR_TRANSMISSION})))
	{
		evalBsdf(
			eval.inputs.X, eval.inputs.L, eval.inputs.V, 
			&(eval.outputs.bsdf));
	}
	else 
	{
		eval.outputs.bsdf.setValues(0.0_r);
	}
}

void SurfaceOptics::genBsdfSample(BsdfSample& sample) const
{
	genBsdfSample(
		sample.inputs.X, sample.inputs.V,
		&(sample.outputs.L), &(sample.outputs.pdfAppliedBsdf));

	const TSidednessAgreement<ESaPolicy::STRICT> sidedness;
	const bool isSameHemisphere = sidedness.isSameHemisphere(sample.inputs.X, sample.inputs.V, sample.outputs.L);
	const bool isOppositeHemisphere = sidedness.isOppositeHemisphere(sample.inputs.X, sample.inputs.V, sample.outputs.L);

	if((isSameHemisphere && m_phenomena.hasAtLeastOne({ESP::DIFFUSE_REFLECTION, ESP::GLOSSY_REFLECTION, ESP::SPECULAR_REFLECTION})) ||
	   (isOppositeHemisphere && m_phenomena.hasAtLeastOne({ESP::GLOSSY_TRANSMISSION, ESP::SPECULAR_TRANSMISSION})))
	{
		// HACK
	}
	else 
	{
		sample.outputs.pdfAppliedBsdf.setValues(0.0_r);
	}
}

void SurfaceOptics::calcBsdfSamplePdf(BsdfPdfQuery& query) const
{
	calcBsdfSamplePdf(
		query.inputs.X, query.inputs.L, query.inputs.V,
		&(query.outputs.sampleDirPdfW));

	const TSidednessAgreement<ESaPolicy::STRICT> sidedness;
	const bool isSameHemisphere = sidedness.isSameHemisphere(query.inputs.X, query.inputs.V, query.inputs.L);
	const bool isOppositeHemisphere = sidedness.isOppositeHemisphere(query.inputs.X, query.inputs.V, query.inputs.L);

	if((isSameHemisphere && m_phenomena.hasAtLeastOne({ESP::DIFFUSE_REFLECTION, ESP::GLOSSY_REFLECTION, ESP::SPECULAR_REFLECTION})) ||
	   (isOppositeHemisphere && m_phenomena.hasAtLeastOne({ESP::GLOSSY_TRANSMISSION, ESP::SPECULAR_TRANSMISSION})))
	{
		// HACK
	}
	else 
	{
		query.outputs.sampleDirPdfW = 0.0_r;
	}
}

}// end namespace ph