#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableTIR.h"
#include "Math/Math.h"
#include "Math/Random.h"

namespace ph
{

namespace
{
	enum class EInterpolationMode
	{
		NEAREST,

		// NOTE: causing artifacts at grazing angles (especially low roughnesses)
		STOCHASTIC_QUADLINEAR
	};

	constexpr EInterpolationMode MODE = EInterpolationMode::NEAREST;
}

const Logger TableTIR::logger(LogSender("TIR Table"));

real TableTIR::sample(const real cosWi, const real alpha, const real relIor) const
{
	// float indices
	real fCosWi  = (m_numCosWi  - 1) * (cosWi  - m_minCosWi ) / (m_maxCosWi  - m_minCosWi );
	real fAlpha  = (m_numAlpha  - 1) * (alpha  - m_minAlpha ) / (m_maxAlpha  - m_minAlpha );
	real fRelIor = (m_numRelIor - 1) * (relIor - m_minRelIor) / (m_maxRelIor - m_minRelIor);

	// ensure float indices stay in the limits
	// (may have inputs exceeding tabled range)
	fCosWi  = Math::clamp(fCosWi,  0.0_r, static_cast<real>(m_numCosWi  - 1));
	fAlpha  = Math::clamp(fAlpha,  0.0_r, static_cast<real>(m_numAlpha  - 1));
	fRelIor = Math::clamp(fRelIor, 0.0_r, static_cast<real>(m_numRelIor - 1));

	if constexpr(MODE == EInterpolationMode::NEAREST)
	{
		// nearest integer indices
		int iCosWi  = static_cast<int>(fCosWi  + 0.5_r);
		int iAlpha  = static_cast<int>(fAlpha  + 0.5_r);
		int iRelIor = static_cast<int>(fRelIor + 0.5_r);

		return m_table[calcIndex(iCosWi, iAlpha, iRelIor)];
	}
	else if constexpr(MODE == EInterpolationMode::STOCHASTIC_QUADLINEAR)
	{
		// target integer indices
		int iCosWi  = static_cast<int>(fCosWi  + Random::genUniformReal_i0_e1());
		int iAlpha  = static_cast<int>(fAlpha  + Random::genUniformReal_i0_e1());
		int iRelIor = static_cast<int>(fRelIor + Random::genUniformReal_i0_e1());

		// ensure indices stay in the limits
		iCosWi  = std::min(iCosWi,  m_numCosWi  - 1);
		iAlpha  = std::min(iAlpha,  m_numAlpha  - 1);
		iRelIor = std::min(iRelIor, m_numRelIor - 1);

		return m_table[calcIndex(iCosWi, iAlpha, iRelIor)];
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0_r;
	}
}

}// end namespace ph