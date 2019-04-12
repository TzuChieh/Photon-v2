#include "Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Core/Texture/TSampler.h"
#include "Core/Texture/TConstantTexture.h"
#include "Common/assertion.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Math/Mapping/CosThetaWeightedUnitHemisphere.h"
#include "Math/TOrthonormalBasis3.h"

namespace ph
{

OrenNayar::OrenNayar(
	const std::shared_ptr<TTexture<SpectralStrength>>& albedo,
	const real                                         sigmaDegrees) :

	OrenNayar(
		albedo, 
		std::make_shared<TConstantTexture<real>>(sigmaDegrees))
{}

OrenNayar::OrenNayar(
	const std::shared_ptr<TTexture<SpectralStrength>>& albedo,
	const std::shared_ptr<TTexture<real>>&             sigmaDegrees) :

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

void OrenNayar::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	PH_ASSERT_MSG(in.elemental == ALL_ELEMENTALS || in.elemental == 0, std::to_string(in.elemental));

	if(!sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		out.bsdf.setValues(0);
		return;
	}

	const SpectralStrength albedo       = TSampler<SpectralStrength>(EQuantity::ECF).sample(*m_albedo, in.X);
	const real             sigmaRadians = math::to_radians(TSampler<real>().sample(*m_sigmaDegrees, in.X));

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

	out.bsdf = albedo * (constant::rcp_pi<real> * (A + B * cosTerm * sinAlpha * tanBeta));
}

void OrenNayar::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	PH_ASSERT_MSG(in.elemental == ALL_ELEMENTALS || in.elemental == 0, std::to_string(in.elemental));

	const Vector3R N = in.X.getShadingNormal();
	PH_ASSERT(N.isFinite());

	Vector3R& L = out.L;
	real pdfW;
	L = CosThetaWeightedUnitHemisphere::map(
		{Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1()}, 
		&pdfW);
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

	BsdfEvaluation bsdfEval;
	bsdfEval.inputs.set(in, out);
	OrenNayar::calcBsdf(bsdfEval.inputs, bsdfEval.outputs, sidedness);
	if(!bsdfEval.outputs.isGood())
	{
		out.setMeasurability(false);
		return;
	}

	out.pdfAppliedBsdf.setValues(bsdfEval.outputs.bsdf.divLocal(pdfW));
	out.setMeasurability(true);
}

void OrenNayar::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	PH_ASSERT_MSG(in.elemental == ALL_ELEMENTALS || in.elemental == 0, std::to_string(in.elemental));

	if(!sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		out.sampleDirPdfW = 0;
		return;
	}

	const Vector3R N = in.X.getShadingNormal();
	out.sampleDirPdfW = in.L.absDot(N) * constant::rcp_pi<real>;
}

}// end namespace ph