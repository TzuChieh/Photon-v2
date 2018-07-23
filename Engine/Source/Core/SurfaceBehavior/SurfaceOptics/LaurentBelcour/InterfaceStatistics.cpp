#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/InterfaceStatistics.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/conversions.h"
#include "Core/SurfaceBehavior/Property/ExactConductorDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"

#include <cmath>

namespace ph
{

bool InterfaceStatistics::addLayer(const LbLayer& layer2)
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

	// evaluate TIR using the decoupling approximation

	const real n10 = (m_layer0.getIorN() / m_layer1.getIorN()).avg();
	const real tir = TIR().sample(m_cosWi, alpha_, n10);
	m_Ri0.addLocal(m_Ti0 * (1.0_r - tir));
	m_Ri0.clampLocal(0.0_r, 1.0_r);
	m_Ti0.mulLocal(tir);

	// terms for multiple scattering
	const SpectralStrength denoTerm    = SpectralStrength(1.0_r) - m_Ri0 * R12;
	const real             denoTermAvg = denoTerm.avg();
	const SpectralStrength R0iTerm     = denoTermAvg > 0.0_r ? (m_T0i * R12 * m_Ti0) / denoTerm : SpectralStrength(0.0_r);
	const SpectralStrength Ri0Term     = denoTermAvg > 0.0_r ? (T21 * m_Ri0 * T12) / denoTerm   : SpectralStrength(0.0_r);
	const SpectralStrength RrTerm      = denoTermAvg > 0.0_r ? (m_Ri0 * R12) / denoTerm         : SpectralStrength(0.0_r);
	const real             R0iTermAvg  = R0iTerm.avg();
	const real             Ri0TermAvg  = Ri0Term.avg();
	const real             RrTermAvg   = RrTerm.avg();
	
	// adding operator on the energy
	const SpectralStrength R0i = m_R0i + R0iTerm;
	const SpectralStrength T0i = (m_T0i * T12) / denoTerm;
	const SpectralStrength Ri0 = R21 + Ri0Term;
	const SpectralStrength Ti0 = (T21 * m_Ti0) / denoTerm;

	// scalar energy terms for adding on variance
	const real R21Avg  = R21.avg();
	const real mR0iAvg = m_R0i.avg();
	const real R0iAvg  = R0i.avg();
	const real Ri0Avg  = Ri0.avg();

	// adding operator on the normalized variance
	real sR0i = (mR0iAvg * m_sR0i + R0iTermAvg * (m_sTi0 + m_J0i * (m_sT0i + sR12 + RrTermAvg * (sR12 + m_sRi0))));/* / R0iAvg */
	real sT0i = j12 * m_sT0i + sT12 + j12 * (sR12 + m_sRi0) * RrTermAvg;
	real sRi0 = (R21Avg * sR21 + Ri0TermAvg * (sT12 + j12 * (sT21 + m_sRi0 + RrTermAvg * (sR12 + m_sRi0))));/* / R0iAvg */
	real sTi0 = m_Ji0 * sT21 + m_sTi0 + m_Ji0 * (sR12 + m_sRi0) * RrTermAvg;
	sR0i = R0iAvg > 0.0_r ? sR0i / R0iAvg : 0.0_r;
	sRi0 = Ri0Avg > 0.0_r ? sRi0 / Ri0Avg : 0.0_r;

	// store the coefficient and variance
	if(R0iTermAvg > 0.0_r)
	{
		m_energyScale     = R0iTerm;
		m_equivalentAlpha = conversions::varianceToAlpha(m_sTi0 + m_J0i * (m_sT0i + sR12 + RrTermAvg * (sR12 + m_sRi0)));
	}
	else
	{
		m_energyScale     = SpectralStrength(0.0_r);
		m_equivalentAlpha = 0.0_r;
	}

	// update energy
	m_R0i = R0i;
	m_T0i = T0i;
	m_Ri0 = Ri0;
	m_Ti0 = Ti0;

	// update mean
	m_cosWi = cosWt;

	// update variance
	m_sR0i = sR0i;
	m_sT0i = sT0i;
	m_sRi0 = sRi0;
	m_sTi0 = sTi0;

	// update jacobian
	m_J0i *= j12;
	m_Ji0 *= j21;

	const bool canAddAnotherLayer = !(layer2.isConductor());
	return canAddAnotherLayer;
}

}// end namespace ph