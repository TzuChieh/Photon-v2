#include "Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Core/Texture/TSampler.h"
#include "Core/Texture/constant_textures.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

namespace ph
{

OrenNayar::OrenNayar(
	const std::shared_ptr<TTexture<math::Spectrum>>& albedo,
	const real                                       sigmaDegrees) :

	OrenNayar(
		albedo, 
		std::make_shared<TConstantTexture<real>>(sigmaDegrees))
{}

OrenNayar::OrenNayar(
	const std::shared_ptr<TTexture<math::Spectrum>>& albedo,
	const std::shared_ptr<TTexture<real>>&           sigmaDegrees) :

	SurfaceOptics(),

	m_albedo      (albedo),
	m_sigmaDegrees(sigmaDegrees)
{
	PH_ASSERT(albedo);
	PH_ASSERT(sigmaDegrees);

	PH_ASSERT_EQ(m_numElementals, 1);
	m_phenomena.set(ESurfacePhenomenon::DiffuseReflection);
}

ESurfacePhenomenon OrenNayar::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_MSG(elemental == ALL_SURFACE_ELEMENTALS || elemental == 0, std::to_string(elemental));

	return ESurfacePhenomenon::DiffuseReflection;
}

/*
	Note that a dark ring can appear (especially on sphere) using qualitative 
	formula (usually the one used on rendering applications). Full version of
	the formula can be considered in order to solve the issue.

	On the other hand, [1] has good explanation of the issue and provides an
	improved version of the model.

	[1] http://mimosa-pudica.net/improved-oren-nayar.html
*/
void OrenNayar::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	PH_ASSERT_MSG(ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == 0, std::to_string(ctx.elemental));

	if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()))
	{
		out.setMeasurability(false);
		return;
	}

	const auto albedo       = TSampler<math::Spectrum>(math::EColorUsage::ECF).sample(*m_albedo, in.getX());
	const real sigmaRadians = math::to_radians(TSampler<real>().sample(*m_sigmaDegrees, in.getX()));

	const auto& shadingBasis = in.getX().getDetail().getShadingBasis();

	const real sigma2    = sigmaRadians * sigmaRadians;
	const real A         = 1.0_r - (sigma2 / (2.0_r * (sigma2 + 0.33_r)));
	const real B         = 0.45_r * sigma2 / (sigma2 + 0.09_r);
	const real sinThetaL = shadingBasis.sinTheta(in.getL());
	const real sinThetaV = shadingBasis.sinTheta(in.getV());

	real cosTerm = 0.0_r;
	if(sinThetaL > 0.0001_r && sinThetaV > 0.0001_r)
	{
		const real sinPhiL = shadingBasis.sinPhi(in.getL());
		const real sinPhiV = shadingBasis.sinPhi(in.getV());
		const real cosPhiL = shadingBasis.cosPhi(in.getL());
		const real cosPhiV = shadingBasis.cosPhi(in.getV());

		// compute max(0, cos(phi_L - phi_V)) and clamp to <= 1
		cosTerm = math::clamp(cosPhiL * cosPhiV + sinPhiL * sinPhiV, 0.0_r, 1.0_r);
	}

	const real absCosThetaL = shadingBasis.absCosTheta(in.getL());
	const real absCosThetaV = shadingBasis.absCosTheta(in.getV());

	// we can determin the size of theta_L and theta_V by comparing their cosines
	real sinAlpha, tanBeta;
	if(absCosThetaL > absCosThetaV)
	{
		sinAlpha = sinThetaV;
		tanBeta  = sinThetaL / absCosThetaL;
	}
	else
	{
		sinAlpha = sinThetaL;
		tanBeta  = sinThetaV / absCosThetaV;
	}

	const math::Spectrum bsdf = 
		albedo * (math::constant::rcp_pi<real> * (A + B * cosTerm * sinAlpha * tanBeta));
	out.setBsdf(bsdf);
}

void OrenNayar::genBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	PH_ASSERT_MSG(ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == 0, std::to_string(ctx.elemental));

	const math::Vector3R N = in.getX().getShadingNormal();
	PH_ASSERT(N.isFinite());

	real pdfW;
	math::Vector3R L = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(
		sampleFlow.flow2D(), &pdfW);
	if(pdfW == 0.0_r)
	{
		out.setMeasurability(false);
		return;
	}

	// Transform to world space and make it on the same hemisphere as V
	L = in.getX().getDetail().getShadingBasis().localToWorld(L);
	L.normalizeLocal();
	if(in.getV().dot(N) < 0.0_r)
	{
		L.mulLocal(-1.0_r);
	}

	BsdfEvalQuery eval;
	eval.inputs.set(in.getX(), L, in.getV());
	OrenNayar::calcBsdf(ctx, eval.inputs, eval.outputs);

	const real absNoL = N.absDot(L);
	const math::Spectrum bsdf = 
		eval.outputs.isMeasurable() ? eval.outputs.getBsdf() : math::Spectrum(0);

	out.setPdfAppliedBsdfCos(bsdf * absNoL / pdfW, absNoL);
	out.setL(L);
}

void OrenNayar::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	PH_ASSERT_MSG(ctx.elemental == ALL_SURFACE_ELEMENTALS || ctx.elemental == 0, std::to_string(ctx.elemental));

	if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()))
	{
		out.setSampleDirPdfW(0);
		return;
	}

	const math::Vector3R N = in.getX().getShadingNormal();
	out.setSampleDirPdfW(in.getL().absDot(N) * math::constant::rcp_pi<real>);
}

}// end namespace ph
