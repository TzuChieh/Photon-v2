#include "Core/SurfaceBehavior/SurfaceOptics/LambertianDiffuse.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Math/Random.h"
#include "Math/constant.h"
#include "Actor/Material/MatteOpaque.h"
#include "Math/sampling.h"
#include "Math/Math.h"
#include "Core/Texture/TSampler.h"
#include "Core/Texture/TConstantTexture.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

LambertianDiffuse::LambertianDiffuse() :
	SurfaceOptics(),
	m_albedo(std::make_shared<TConstantTexture<SpectralStrength>>(SpectralStrength(0.5_r)))
{
	m_phenomena.set({ESP::DIFFUSE_REFLECTION});
}

LambertianDiffuse::~LambertianDiffuse() = default;

void LambertianDiffuse::setAlbedo(
	const std::shared_ptr<TTexture<SpectralStrength>>& albedo)
{
	m_albedo = albedo;
}

void LambertianDiffuse::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	const real NoL = X.getShadingNormal().dot(L);
	const real NoV = X.getShadingNormal().dot(V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		out_bsdf->setValues(0);
		return;
	}

	SpectralStrength albedo = TSampler<SpectralStrength>(EQuantity::ECF).sample(*m_albedo, X);

	*out_bsdf = albedo.divLocal(PH_PI_REAL);
}

void LambertianDiffuse::genBsdfSample(
	const SurfaceHit& X, const Vector3R& V,
	Vector3R* const         out_L, 
	SpectralStrength* const out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	// Lambertian diffuse model's BRDF is simply albedo/pi.
	// The importance sampling strategy is to use the cosine term in the rendering equation, 
	// generating a cos(theta) weighted L corresponding to N, which PDF is cos(theta)/pi.
	// Thus, BRDF_lambertian/PDF = albedo/cos(theta).

	SpectralStrength albedo = TSampler<SpectralStrength>(EQuantity::ECF).sample(*m_albedo, X);

	// generate and transform L to N's space

	const Vector3R& N = X.getShadingNormal();
	PH_ASSERT(N.isFinite());

	Vector3R& L = *out_L;
	sampling::unit_hemisphere::cosine_theta_weighted::gen(
		Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1(), &L);
	Vector3R u;
	Vector3R v(N);
	Vector3R w;
	Math::formOrthonormalBasis(v, &u, &w);
	L = u.mulLocal(L.x).addLocal(v.mulLocal(L.y)).addLocal(w.mulLocal(L.z));
	L.normalizeLocal();
	if(V.dot(N) < 0.0_r)
	{
		L.mulLocal(-1.0_r);
	}

	const real absNoL = N.absDot(L);
	if(absNoL == 0.0_r)
	{
		out_pdfAppliedBsdf->setValues(0);
		return;
	}

	out_pdfAppliedBsdf->setValues(albedo.mulLocal(1.0_r / absNoL));
}

void LambertianDiffuse::calcBsdfSamplePdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V, 
	real* const out_pdfW) const
{
	PH_ASSERT(out_pdfW);

	const Vector3R& N = X.getShadingNormal();
	*out_pdfW = L.absDot(N) * PH_RECI_PI_REAL;
}

}// end namespace ph