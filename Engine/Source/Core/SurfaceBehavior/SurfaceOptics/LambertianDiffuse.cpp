#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Actor/Material/MatteOpaque.h"
#include "Math/sampling.h"
#include "Math/math.h"
#include "Core/Texture/TSampler.h"
#include "Core/Texture/TConstantTexture.h"
#include "Common/assertion.h"
#include "Core/SidednessAgreement.h"

#include <cmath>

namespace ph
{

LambertianDiffuse::LambertianDiffuse(const std::shared_ptr<TTexture<SpectralStrength>>& albedo) :
	SurfaceOptics(),
	m_albedo(albedo)
{
	PH_ASSERT(albedo);

	m_phenomena.set({ESP::DIFFUSE_REFLECTION});
}

void LambertianDiffuse::calcBsdf(
	const BsdfEvaluation::Input& in,
	BsdfEvaluation::Output&      out,
	const SidednessAgreement&    sidedness) const
{
	if(!sidedness.isSameHemisphere(in.X, in.L, in.V))
	{
		out.bsdf.setValues(0);
		return;
	}

	SpectralStrength albedo = TSampler<SpectralStrength>(EQuantity::ECF).sample(*m_albedo, in.X);
	out.bsdf = albedo.divLocal(PH_PI_REAL);
}

void LambertianDiffuse::calcBsdfSample(
	const BsdfSample::Input&  in,
	BsdfSample::Output&       out,
	const SidednessAgreement& sidedness) const
{
	// Lambertian diffuse model's BRDF is simply albedo/pi.
	// The importance sampling strategy is to use the cosine term in the rendering equation, 
	// generating a cos(theta) weighted L corresponding to N, which PDF is cos(theta)/pi.
	// Thus, BRDF_lambertian/PDF = albedo/cos(theta).

	SpectralStrength albedo = TSampler<SpectralStrength>(EQuantity::ECF).sample(*m_albedo, in.X);

	// generate and transform L to N's space

	const Vector3R& N = in.X.getShadingNormal();
	PH_ASSERT(N.isFinite());

	Vector3R& L = out.L;
	sampling::unit_hemisphere::cosine_theta_weighted::gen(
		Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1(), &L);
	Vector3R u;
	Vector3R v(N);
	Vector3R w;
	math::form_orthonormal_basis(v, &u, &w);
	L = u.mulLocal(L.x).addLocal(v.mulLocal(L.y)).addLocal(w.mulLocal(L.z));
	L.normalizeLocal();
	if(in.V.dot(N) < 0.0_r)
	{
		L.mulLocal(-1.0_r);
	}

	// DEBUG
	/*if(V.dot(N) < 0.0_r)
	{
		out_pdfAppliedBsdf->setValues(0);
		return;
	}*/

	const real absNoL = N.absDot(L);
	if(absNoL == 0.0_r)
	{
		out.pdfAppliedBsdf.setValues(0);
		return;
	}

	out.pdfAppliedBsdf.setValues(albedo.mulLocal(1.0_r / absNoL));
}

void LambertianDiffuse::calcBsdfSamplePdfW(
	const BsdfPdfQuery::Input& in,
	BsdfPdfQuery::Output&      out,
	const SidednessAgreement&  sidedness) const
{
	const Vector3R& N = in.X.getShadingNormal();
	out.sampleDirPdfW = in.L.absDot(N) * PH_RECI_PI_REAL;
}

}// end namespace ph