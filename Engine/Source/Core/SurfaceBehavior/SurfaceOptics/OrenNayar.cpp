#include "Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/TVector3.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Core/Texture/TSampler.h"
#include "Core/Texture/TConstantTexture.h"
#include "Common/assertion.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/TOrthonormalBasis3.h"
#include "Core/SampleGenerator/SampleFlow.h"

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
	m_phenomena.set({ESurfacePhenomenon::DIFFUSE_REFLECTION});
}

ESurfacePhenomenon OrenNayar::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_MSG(elemental == ALL_ELEMENTALS || elemental == 0, std::to_string(elemental));

	return ESurfacePhenomenon::DIFFUSE_REFLECTION;
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
	PH_ASSERT_MSG(ctx.elemental == ALL_ELEMENTALS || ctx.elemental == 0, std::to_string(ctx.elemental));

	if(!ctx.sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		out.bsdf.setColorValues(0);
		return;
	}

	const auto albedo       = TSampler<math::Spectrum>(math::EColorUsage::ECF).sample(*m_albedo, in.X);
	const real sigmaRadians = math::to_radians(TSampler<real>().sample(*m_sigmaDegrees, in.X));

	const auto& shadingBasis = in.X.getDetail().getShadingBasis();

	const real sigma2    = sigmaRadians * sigmaRadians;
	const real A         = 1.0_r - (sigma2 / (2.0_r * (sigma2 + 0.33_r)));
	const real B         = 0.45_r * sigma2 / (sigma2 + 0.09_r);
	const real sinThetaL = shadingBasis.sinTheta(in.L);
	const real sinThetaV = shadingBasis.sinTheta(in.V);

	real cosTerm = 0.0_r;
	if(sinThetaL > 0.0001_r && sinThetaV > 0.0001_r)
	{
		const real sinPhiL = shadingBasis.sinPhi(in.L);
		const real sinPhiV = shadingBasis.sinPhi(in.V);
		const real cosPhiL = shadingBasis.cosPhi(in.L);
		const real cosPhiV = shadingBasis.cosPhi(in.V);

		// compute max(0, cos(phi_L - phi_V)) and clamp to <= 1
		cosTerm = math::clamp(cosPhiL * cosPhiV + sinPhiL * sinPhiV, 0.0_r, 1.0_r);
	}

	const real absCosThetaL = shadingBasis.absCosTheta(in.L);
	const real absCosThetaV = shadingBasis.absCosTheta(in.V);

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

	out.bsdf = albedo * (math::constant::rcp_pi<real> * (A + B * cosTerm * sinAlpha * tanBeta));
}

void OrenNayar::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	PH_ASSERT_MSG(ctx.elemental == ALL_ELEMENTALS || ctx.elemental == 0, std::to_string(ctx.elemental));

	const math::Vector3R N = in.X.getShadingNormal();
	PH_ASSERT(N.isFinite());

	math::Vector3R& L = out.L;
	real pdfW;
	L = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(sampleFlow.flow2D(), &pdfW);
	if(pdfW == 0.0_r)
	{
		out.setMeasurability(false);
		return;
	}

	// transform to world space and make it on the same hemisphere as V
	L = in.X.getDetail().getShadingBasis().localToWorld(L);
	L.normalizeLocal();
	if(in.V.dot(N) < 0.0_r)
	{
		L.mulLocal(-1.0_r);
	}

	const real absNoL = N.absDot(L);
	if(absNoL == 0.0_r)
	{
		out.setMeasurability(false);
		return;
	}

	BsdfEvalQuery eval;
	eval.inputs.set(in.X, L, in.V);
	OrenNayar::calcBsdf(ctx, eval.inputs, eval.outputs);
	if(!eval.outputs.isGood())
	{
		out.setMeasurability(false);
		return;
	}

	out.pdfAppliedBsdf = eval.outputs.bsdf.div(pdfW);
	out.setMeasurability(true);
}

void OrenNayar::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	PH_ASSERT_MSG(ctx.elemental == ALL_ELEMENTALS || ctx.elemental == 0, std::to_string(ctx.elemental));

	if(!ctx.sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		out.sampleDirPdfW = 0;
		return;
	}

	const math::Vector3R N = in.X.getShadingNormal();
	out.sampleDirPdfW = in.L.absDot(N) * math::constant::rcp_pi<real>;
}

}// end namespace ph
