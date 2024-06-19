#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableTIR.h"
#include "Math/math.h"
#include "Math/Random/Random.h"

namespace ph
{

namespace
{

enum class EInterpolationMode
{
	Nearest,

	// NOTE: causing artifacts at grazing angles (especially low roughnesses)
	StochasticQuadlinear
};

constexpr EInterpolationMode MODE = EInterpolationMode::Nearest;

}

real TableTIR::sample(const real cosWi, const real alpha, const real relIor) const
{
	// Float indices
	real fCosWi  = (m_numCosWi  - 1) * (cosWi  - m_minCosWi ) / (m_maxCosWi  - m_minCosWi );
	real fAlpha  = (m_numAlpha  - 1) * (alpha  - m_minAlpha ) / (m_maxAlpha  - m_minAlpha );
	real fRelIor = (m_numRelIor - 1) * (relIor - m_minRelIor) / (m_maxRelIor - m_minRelIor);

	// Ensure float indices stay in the limits
	// (may have inputs exceeding tabled range)
	fCosWi  = math::clamp(fCosWi,  0.0_r, static_cast<real>(m_numCosWi  - 1));
	fAlpha  = math::clamp(fAlpha,  0.0_r, static_cast<real>(m_numAlpha  - 1));
	fRelIor = math::clamp(fRelIor, 0.0_r, static_cast<real>(m_numRelIor - 1));

	if constexpr(MODE == EInterpolationMode::Nearest)
	{
		// Nearest integer indices
		int iCosWi  = static_cast<int>(fCosWi  + 0.5_r);
		int iAlpha  = static_cast<int>(fAlpha  + 0.5_r);
		int iRelIor = static_cast<int>(fRelIor + 0.5_r);

		return m_table[calcIndex(iCosWi, iAlpha, iRelIor)];
	}
	else if constexpr(MODE == EInterpolationMode::StochasticQuadlinear)
	{
		// Target integer indices
		int iCosWi  = static_cast<int>(fCosWi  + math::Random::sample());
		int iAlpha  = static_cast<int>(fAlpha  + math::Random::sample());
		int iRelIor = static_cast<int>(fRelIor + math::Random::sample());

		// Ensure indices stay in the limits
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
