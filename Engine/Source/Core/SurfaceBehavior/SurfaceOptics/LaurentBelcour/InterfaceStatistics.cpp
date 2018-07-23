#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/InterfaceStatistics.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/conversions.h"
#include "Core/SurfaceBehavior/Property/ExactConductorDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"

#include <cmath>

namespace ph
{

void InterfaceStatistics::appendLayer(const LbLayer& layer2)
{
	const SpectralStrength iorN12 = layer2.getIorN() / m_layer1.getIorN();
	const SpectralStrength iorK12 = layer2.getIorK() / m_layer1.getIorN();

	const real n12 = iorN12.avg();

	SpectralStrength R12, T12, R21, T21;
	real sR12 = 0.0_r, sT12 = 0.0_r, sR21 = 0.0_r, sT21 = 0.0_r;
	real j12  = 1.0_r, j21  = 1.0_r;

	real cosWt = -1.0_r;

	// not being block by conductor
	bool hasTransmission = layer2.getIorK().isZero();
	if(hasTransmission)
	{
		// refraction
		const real sinWi = std::sqrt(1.0_r - m_cosWi * m_cosWi);
		const real sinWt = sinWi / n12;
		if(sinWi <= 1.0_r)
		{
			cosWt = std::sqrt(1.0_r - sinWt * sinWt);
		}
		else
		{
			// total internal reflection occurred
			hasTransmission = false;
		}
	}

	// reflection variance terms
	sR12 = conversions::alphaToVariance(layer2.getAlpha());
	sR21 = sR12;

	// transmission variance terms
	if(hasTransmission)
	{
		// NOTE: this part is vastly different from the paper, but is how it was
		// implemented in the reference code

		const real cosWt_ = 1.0_r;
		const real cosWi_ = 1.0_r;

		sT12 = conversions::alphaToVariance(
			layer2.getAlpha() * 0.5_r * std::abs((cosWt_ * n12 - cosWi_)) / (cosWt_ * n12));
		sT21 = conversions::alphaToVariance(
			layer2.getAlpha() * 0.5_r * std::abs((cosWi_ / n12 - cosWt_)) / (cosWi_ / n12));
		j12 = (cosWt   / m_cosWi) * n12;
		j21 = (m_cosWi / cosWt  ) / n12;
	}

	// evaluate tables using a modified roughness accounting for top layers
	const real alpha_ = conversions::varianceToAlpha(m_sT0i + sR12);

	R12 = FGD().sample(m_cosWi, alpha_, iorN12, iorK12);
	T12 = iorK12.isZero() ? SpectralStrength(1.0_r) - R12 : SpectralStrength(0.0_r);
	if(hasTransmission)
	{
		R21 = R12;
		T21 = T12;
	}
	else
	{
		R21 = SpectralStrength(0.0_r);
		T21 = SpectralStrength(0.0_r);
	}

	
	// TODO

}

}// end namespace ph