#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableFGD.h"
#include "Math/math.h"
#include "Math/Random.h"

#include <cmath>
#include <array>

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

const Logger TableFGD::logger(LogSender("FGD Table"));

real TableFGD::sample(const real cosWi, const real alpha, const real iorN, const real iorK) const
{
	// float indices
	real fCosWi = (m_numCosWi - 1) * (cosWi - m_minCosWi) / (m_maxCosWi - m_minCosWi);
	real fAlpha = (m_numAlpha - 1) * (alpha - m_minAlpha) / (m_maxAlpha - m_minAlpha);
	real fIorN  = (m_numIorN  - 1) * (iorN  - m_minIorN ) / (m_maxIorN  - m_minIorN );
	real fIorK  = (m_numIorK  - 1) * (iorK  - m_minIorK ) / (m_maxIorK  - m_minIorK );

	// ensure float indices stay in the limits
	// (may have inputs exceeding tabled range)
	fCosWi = math::clamp(fCosWi, 0.0_r, static_cast<real>(m_numCosWi - 1));
	fAlpha = math::clamp(fAlpha, 0.0_r, static_cast<real>(m_numAlpha - 1));
	fIorN  = math::clamp(fIorN,  0.0_r, static_cast<real>(m_numIorN  - 1));
	fIorK  = math::clamp(fIorK,  0.0_r, static_cast<real>(m_numIorK  - 1));

	if constexpr(MODE == EInterpolationMode::NEAREST)
	{
		// nearest integer indices
		int iCosWi = static_cast<int>(fCosWi + 0.5_r);
		int iAlpha = static_cast<int>(fAlpha + 0.5_r);
		int iIorN  = static_cast<int>(fIorN  + 0.5_r);
		int iIorK  = static_cast<int>(fIorK  + 0.5_r);

		return m_table[calcIndex(iCosWi, iAlpha, iIorN, iIorK)];
	}
	else if constexpr(MODE == EInterpolationMode::STOCHASTIC_QUADLINEAR)
	{
		// target integer indices
		int iCosWi = static_cast<int>(fCosWi + math::Random::genUniformReal_i0_e1());
		int iAlpha = static_cast<int>(fAlpha + math::Random::genUniformReal_i0_e1());
		int iIorN  = static_cast<int>(fIorN  + math::Random::genUniformReal_i0_e1());
		int iIorK  = static_cast<int>(fIorK  + math::Random::genUniformReal_i0_e1());

		// ensure indices stay in the limits
		iCosWi = std::min(iCosWi, m_numCosWi - 1);
		iAlpha = std::min(iAlpha, m_numAlpha - 1);
		iIorN  = std::min(iIorN,  m_numIorN  - 1);
		iIorK  = std::min(iIorK,  m_numIorK  - 1);

		return m_table[calcIndex(iCosWi, iAlpha, iIorN, iIorK)];
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0_r;
	}
}

void TableFGD::downSampleHalf()
{
	int newNumCosWi = m_numCosWi / 2;
	int newNumAlpha = m_numAlpha / 2;
	int newNumIorN  = m_numIorN  / 2;
	int newNumIorK  = m_numIorK  / 2;

	std::vector<float> newTable(
		newNumCosWi * newNumAlpha * newNumIorN * newNumIorK, 0.0f);

	for(int iCosWi = 0; iCosWi < m_numCosWi; iCosWi += 2)
	{
		for(int iAlpha = 0; iAlpha < m_numAlpha; iAlpha += 2)
		{
			for(int iIorN = 0; iIorN < m_numIorN; iIorN += 2)
			{
				for(int iIorK = 0; iIorK < m_numIorK; iIorK += 2)
				{
					int oldIndex = iIorK + m_numIorK * (iIorN + m_numIorN * (iAlpha + m_numAlpha * iCosWi));
					int newIndex = (iIorK / 2) + newNumIorK * ((iIorN / 2) + newNumIorN * ((iAlpha / 2) + newNumAlpha * (iCosWi / 2)));
					newTable[newIndex] = m_table[oldIndex];
				}
			}
		}
	}

	m_numCosWi = newNumCosWi;
	m_numAlpha = newNumAlpha;
	m_numIorN  = newNumIorN;
	m_numIorK  = newNumIorK;

	m_table = newTable;
}

}// end namespace ph
