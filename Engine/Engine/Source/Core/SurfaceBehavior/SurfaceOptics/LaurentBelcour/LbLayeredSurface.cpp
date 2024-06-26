#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "Core/SurfaceBehavior/BsdfHelper.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/InterfaceStatistics.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayer.h"
#include "Core/SurfaceBehavior/Property/IsoTrowbridgeReitzConstant.h"
#include "Core/LTA/SidednessAgreement.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Math/Random/Random.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph
{

thread_local std::vector<real> LbLayeredSurface::sampleWeights;
thread_local std::vector<real> LbLayeredSurface::alphas;

LbLayeredSurface::LbLayeredSurface(
	const std::vector<math::Spectrum>& iorNs,
	const std::vector<math::Spectrum>& iorKs,
	const std::vector<real>&           alphas,
	const std::vector<real>&           depths,
	const std::vector<real>&           gs,
	const std::vector<math::Spectrum>& sigmaAs,
	const std::vector<math::Spectrum>& sigmaSs) :

	SurfaceOptics(),

	m_iorNs(iorNs), m_iorKs(iorKs), 
	m_alphas(alphas), 
	m_depths(depths), 
	m_gs(gs), 
	m_sigmaAs(sigmaAs), m_sigmaSs(sigmaSs)
{
	PH_ASSERT(m_iorNs.size() != 0);

	PH_ASSERT(m_iorNs.size()   == m_iorKs.size()   && 
	          m_iorKs.size()   == m_alphas.size()  &&
	          m_alphas.size()  == m_depths.size()  &&
	          m_depths.size()  == m_gs.size()      &&
	          m_gs.size()      == m_sigmaAs.size() &&
	          m_sigmaAs.size() == m_sigmaSs.size());

	m_phenomena.set(ESurfacePhenomenon::GlossyReflection);
}

ESurfacePhenomenon LbLayeredSurface::getPhenomenonOf(const SurfaceElemental elemental) const
{
	PH_ASSERT_EQ(elemental, 0);

	return ESurfacePhenomenon::GlossyReflection;
}

void LbLayeredSurface::calcBsdf(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()))
	{
		out.setMeasurability(false);
		return;
	}

	const math::Vector3R N = in.getX().getShadingNormal();
	const real NoL = N.dot(in.getL());
	const real NoV = N.dot(in.getV());
	const real brdfDeno = 4.0_r * std::abs(NoV * NoL);
	if(brdfDeno == 0.0_r)
	{
		out.setMeasurability(false);
		return;
	}

	math::Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(in.getL(), in.getV(), N, &H))
	{
		out.setMeasurability(false);
		return;
	}
	
	const real absHoL = std::min(H.absDot(in.getL()), 1.0_r);

	// Using the half angle symmetric model
	InterfaceStatistics statistics(absHoL, LbLayer());
	math::Spectrum bsdf(0);
	for(std::size_t i = 0; i < numLayers(); ++i)
	{
		const LbLayer addedLayer = getLayer(i, statistics.getLastLayer());
		if(!statistics.addLayer(addedLayer))
		{
			PH_ASSERT(i == numLayers() - 1);
		}

		const IsoTrowbridgeReitzConstant ggx(statistics.getEquivalentAlpha());
		const real D = ggx.distribution(in.getX(), N, H);
		const real G = ggx.shadowing(in.getX(), N, H, in.getL(), in.getV());

		bsdf += statistics.getEnergyScale().mul(D * G / brdfDeno);
	}
	out.setBsdf(bsdf);
}

void LbLayeredSurface::genBsdfSample(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	const math::Vector3R N = in.getX().getShadingNormal();
	const real absNoV = std::min(N.absDot(in.getV()), 1.0_r);

	// Perform adding-doubling algorithm and gather information for later sampling process:
	// we first construct an approximative distribution from the view direction, then use it
	// to importance sample the actual BSDF we need.
	
	sampleWeights.resize(numLayers());
	alphas.resize(numLayers());

	real summedSampleWeights = 0.0_r;
	InterfaceStatistics statistics(absNoV, LbLayer());
	for(std::size_t i = 0; i < numLayers(); ++i)
	{
		const LbLayer addedLayer = getLayer(i, statistics.getLastLayer());
		if(!statistics.addLayer(addedLayer))
		{
			PH_ASSERT(i == numLayers() - 1);
		}

		const real sampleWeight = statistics.getEnergyScale().avg();
		sampleWeights[i] = sampleWeight;
		summedSampleWeights += sampleWeight;

		alphas[i] = statistics.getEquivalentAlpha();
	}

	// Select BSDF lobe to sample based on energy term. Note that we must watch out for
	// the case where `selectWeight` cannot be reduced to <= 0 due to numerical error
	// (handled in current implmentation).
	// TODO: try to use sampleFlow for this
	real selectWeight = math::Random::sample() * summedSampleWeights - sampleWeights[0];
	std::size_t selectIndex = 0;
	for(selectIndex = 0; selectWeight > 0.0_r && selectIndex + 1 < numLayers(); ++selectIndex)
	{
		selectWeight -= sampleWeights[selectIndex + 1];
	}
	PH_ASSERT_MSG(selectIndex < numLayers(), 
		"selectIndex  = " + std::to_string(selectIndex)  + "\n"
		"selectWeight = " + std::to_string(selectWeight) + "\n");

	const IsoTrowbridgeReitzConstant selectedGgx(alphas[selectIndex]);

	math::Vector3R H;
	selectedGgx.genDistributedH(in.getX(), N, sampleFlow.flow2D(), &H);

	const math::Vector3R L = in.getV().mul(-1.0_r).reflect(H).normalizeLocal();
	if(!ctx.sidedness.isSameHemisphere(in.getX(), L, in.getV()))
	{
		out.setMeasurability(false);
		return;
	}

	const real NoH = N.dot(H);
	const real HoL = H.dot(L);

	// MIS with balance heuristic
	real pdf = 0.0_r;
	for(std::size_t i = 0; i < numLayers(); ++i)
	{
		const IsoTrowbridgeReitzConstant ggx(alphas[i]);
		const real D = ggx.distribution(in.getX(), N, H);
		const real weight = sampleWeights[i] / summedSampleWeights;
		pdf += weight * std::abs(D * NoH / (4.0_r * HoL));
	}

	BsdfEvalInput evalInput;
	evalInput.set(in.getX(), L, in.getV());

	BsdfEvalOutput evalOutput;
	LbLayeredSurface::calcBsdf(ctx, evalInput, evalOutput);

	const real absNoL = N.absDot(L);
	const math::Spectrum bsdf = 
		evalOutput.isMeasurable() ? evalOutput.getBsdf() : math::Spectrum(0);

	out.setPdfAppliedBsdfCos(bsdf * absNoL / pdf, absNoL);
	out.setL(L);
}

void LbLayeredSurface::calcBsdfPdf(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	out.setSampleDirPdf({});

	if(!ctx.sidedness.isSameHemisphere(in.getX(), in.getL(), in.getV()))
	{
		return;
	}

	const math::Vector3R N = in.getX().getShadingNormal();

	math::Vector3R H;
	if(!BsdfHelper::makeHalfVectorSameHemisphere(in.getL(), in.getV(), N, &H))
	{
		return;
	}

	const real absNoV = std::min(N.absDot(in.getV()), 1.0_r);
	const real NoH = N.dot(H);
	const real HoL = H.dot(in.getL());

	// Similar to genBsdfSample(), here we perform adding-doubling then compute
	// MIS'ed (balance heuristic) PDF value.
	//
	real summedSampleWeights = 0.0_r;
	real pdf = 0.0_r;
	InterfaceStatistics statistics(absNoV, LbLayer());
	for(std::size_t i = 0; i < numLayers(); ++i)
	{
		const LbLayer addedLayer = getLayer(i, statistics.getLastLayer());
		if(!statistics.addLayer(addedLayer))
		{
			PH_ASSERT(i == numLayers() - 1);
		}

		const real sampleWeight = statistics.getEnergyScale().avg();
		summedSampleWeights += sampleWeight;

		IsoTrowbridgeReitzConstant ggx(statistics.getEquivalentAlpha());
		const real D = ggx.distribution(in.getX(), N, H);
		pdf += sampleWeight * std::abs(D * NoH / (4.0_r * HoL));
	}

	if(summedSampleWeights > 0.0_r && std::isfinite(pdf))
	{
		out.setSampleDirPdf(lta::PDF::W(pdf / summedSampleWeights));
	}
}

LbLayer LbLayeredSurface::getLayer(const std::size_t layerIndex, const LbLayer& previousLayer) const
{
	PH_ASSERT(layerIndex < numLayers());

	const real depth = m_depths[layerIndex];
	if(depth == 0.0_r)
	{
		return LbLayer(
			m_alphas[layerIndex], 
			m_iorNs[layerIndex], 
			m_iorKs[layerIndex]);
	}
	else
	{
		return LbLayer(
			m_gs[layerIndex], 
			depth, 
			m_sigmaAs[layerIndex], 
			m_sigmaSs[layerIndex], 
			previousLayer);
	}
}

}// end namespace ph
