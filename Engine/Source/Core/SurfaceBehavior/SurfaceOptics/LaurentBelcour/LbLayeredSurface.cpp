#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "Common/assertion.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/InterfaceStatistics.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayer.h"
#include "Math/Random.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitz.h"

#include <cmath>

namespace ph
{

thread_local std::vector<real> LbLayeredSurface::sampleWeights;
thread_local std::vector<real> LbLayeredSurface::alphas;

LbLayeredSurface::LbLayeredSurface(
	const std::vector<SpectralStrength>& iorNs,
	const std::vector<SpectralStrength>& iorKs,
	const std::vector<real>&             alphas) :

	SurfaceOptics(),

	m_iorNs(iorNs), m_iorKs(iorKs), m_alphas(alphas)
{
	PH_ASSERT(m_iorNs.size() != 0);
	PH_ASSERT(m_iorNs.size() == m_iorKs.size() && m_iorKs.size() == m_alphas.size());

	m_phenomena.set({ESP::GLOSSY_REFLECTION});
}

LbLayeredSurface::~LbLayeredSurface() = default;

void LbLayeredSurface::evalBsdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	SpectralStrength* const out_bsdf) const
{
	PH_ASSERT(out_bsdf);

	out_bsdf->setValues(0);

	const Vector3R& N = X.getShadingNormal();

	const real NoL = N.dot(L);
	const real NoV = N.dot(V);

	// check if L, V lies on different side of the surface
	if(NoL * NoV <= 0.0_r)
	{
		return;
	}

	const real brdfDeno = 4.0_r * std::abs(NoV * NoL);
	if(brdfDeno == 0.0_r)
	{
		return;
	}

	Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(L, V, N, &H))
	{
		return;
	}
	
	const real absHoL = std::min(H.absDot(L), 1.0_r);

	InterfaceStatistics statistics(absHoL, LbLayer(0, SpectralStrength(1)));
	for(std::size_t i = 0; i < m_alphas.size(); ++i)
	{
		const LbLayer addedLayer(m_alphas[i], m_iorNs[i], m_iorKs[i]);
		if(!statistics.addLayer(addedLayer))
		{
			PH_ASSERT(i == m_alphas.size() - 1);
		}

		IsoTrowbridgeReitz ggx(statistics.getEquivalentAlpha());
		const real D = ggx.distribution(X, N, H);
		const real G = ggx.shadowing(X, N, H, L, V);

		out_bsdf->addLocal(statistics.getEnergyScale().mul(D * G / brdfDeno));
	}
}

void LbLayeredSurface::genBsdfSample(
	const SurfaceHit& X, const Vector3R& V,
	Vector3R* const         out_L,
	SpectralStrength* const out_pdfAppliedBsdf) const
{
	PH_ASSERT(out_L && out_pdfAppliedBsdf);

	out_pdfAppliedBsdf->setValues(0);

	const Vector3R& N = X.getShadingNormal();
	const real absNoV = std::min(N.absDot(V), 1.0_r);

	sampleWeights.resize(numLayers());
	alphas.resize(numLayers());

	real summedSampleWeights = 0.0_r;
	InterfaceStatistics statistics(absNoV, LbLayer(0, SpectralStrength(1)));
	for(std::size_t i = 0; i < numLayers(); ++i)
	{
		const LbLayer addedLayer(m_alphas[i], m_iorNs[i], m_iorKs[i]);
		if(!statistics.addLayer(addedLayer))
		{
			PH_ASSERT(i == numLayers() - 1);
		}

		const real sampleWeight = statistics.getEnergyScale().avg();
		sampleWeights[i] = sampleWeight;
		summedSampleWeights += sampleWeight;

		alphas[i] = statistics.getEquivalentAlpha();
	}

	real selectWeight = Random::genUniformReal_i0_e1() * summedSampleWeights - sampleWeights[0];
	std::size_t selectIndex = 0;
	for(selectIndex = 0; selectWeight > 0.0_r && selectIndex < numLayers(); ++selectIndex)
	{
		selectWeight -= sampleWeights[selectIndex + 1];
	}
	PH_ASSERT(selectIndex < numLayers());

	IsoTrowbridgeReitz ggx(alphas[selectIndex]);
	Vector3R H;
	ggx.genDistributedH(X, Random::genUniformReal_i0_e1(), Random::genUniformReal_i0_e1(), N, &H);
	const Vector3R L = V.mul(-1.0_r).reflect(H).normalizeLocal();
	*out_L = L;

	const real NoH = N.dot(H);
	const real HoL = H.dot(L);

	real pdf = 0.0_r;
	for(std::size_t i = 0; i < numLayers(); ++i)
	{
		IsoTrowbridgeReitz ggx(alphas[i]);
		const real D = ggx.distribution(X, N, H);
		const real G = ggx.shadowing(X, N, H, L, V);

		const real weight = sampleWeights[i] / summedSampleWeights;
		pdf += weight * std::abs(D * NoH / (4.0_r * HoL));
	}

	// TEST
	if(pdf == 0.0_r || (4.0_r * HoL) == 0.0_r)
	{
		return;
	}

	SpectralStrength bsdf;
	LbLayeredSurface::evalBsdf(X, L, V, &bsdf);
	*out_pdfAppliedBsdf = bsdf / pdf;
}

void LbLayeredSurface::calcBsdfSamplePdf(
	const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
	real* out_pdfW) const
{


	// TODO
}

}// end namespace ph