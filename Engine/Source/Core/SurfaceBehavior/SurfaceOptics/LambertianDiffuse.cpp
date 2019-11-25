#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Math/math.h"
#include "Core/Texture/TSampler.h"
#include "Core/Texture/TConstantTexture.h"
#include "Common/assertion.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"
#include "Math/Geometry/THemisphere.h"
#include "Math/TOrthonormalBasis3.h"

#include <cmath>

namespace ph
{

LambertianDiffuse::LambertianDiffuse(const std::shared_ptr<TTexture<SpectralStrength>>& albedo) :
	SurfaceOptics(),
	m_albedo(albedo)
{
	PH_ASSERT(albedo);

	m_phenomena.set({ESurfacePhenomenon::DIFFUSE_REFLECTION});
}

ESurfacePhenomenon LambertianDiffuse::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::DIFFUSE_REFLECTION;
}

void LambertianDiffuse::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	if(!ctx.sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		out.bsdf.setValues(0);
		return;
	}

	SpectralStrength albedo = TSampler<SpectralStrength>(EQuantity::ECF).sample(*m_albedo, in.X);
	out.bsdf = albedo.mulLocal(math::constant::rcp_pi<real>);
}

void LambertianDiffuse::calcBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	BsdfSample              sample,
	BsdfSampleOutput&       out) const
{
	// Lambertian diffuse model's BRDF is simply albedo/pi.
	// The importance sampling strategy is to use the cosine term in the rendering equation, 
	// generating a cos(theta) weighted L corresponding to N, which PDF is cos(theta)/pi.
	// Thus, BRDF_lambertian/PDF = albedo/cos(theta).

	SpectralStrength albedo = TSampler<SpectralStrength>(EQuantity::ECF).sample(*m_albedo, in.X);

	// generate and transform L to N's space

	const math::Vector3R N = in.X.getShadingNormal();
	PH_ASSERT(N.isFinite());

	math::Vector3R& L = out.L;

	L = math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(
		{math::Random::genUniformReal_i0_e1(), math::Random::genUniformReal_i0_e1()});

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

	out.pdfAppliedBsdf.setValues(albedo.mulLocal(1.0_r / absNoL));
	out.setMeasurability(true);
}

void LambertianDiffuse::calcBsdfSamplePdfW(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	if(!ctx.sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		out.sampleDirPdfW = 0;
		return;
	}

	const math::Vector3R N = in.X.getShadingNormal();
	out.sampleDirPdfW = in.L.absDot(N) * math::constant::rcp_pi<real>;
}

}// end namespace ph
