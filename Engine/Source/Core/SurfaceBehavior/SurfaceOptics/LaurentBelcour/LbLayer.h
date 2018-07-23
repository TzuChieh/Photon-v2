#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"

namespace ph
{

class LbLayer
{
public:
	explicit LbLayer(const SpectralStrength& iorN);
	LbLayer(real alpha, const SpectralStrength& iorN);

	real getAlpha() const;
	const SpectralStrength& getIorN() const;
	const SpectralStrength& getIorK() const;

private:
	static constexpr real INVALID_ALPHA = -1.0_r;

	real             m_alpha;
	SpectralStrength m_iorN, m_iorK;
};

// In-header Implementations:

inline LbLayer::LbLayer(const SpectralStrength& iorN) : 
	LbLayer(INVALID_ALPHA, iorN)
{}

inline LbLayer::LbLayer(const real alpha, const SpectralStrength& iorN) : 
	m_alpha(alpha), 
	m_iorN(iorN), m_iorK(0)
{}

inline real LbLayer::getAlpha() const
{
	return m_alpha;
}

inline const SpectralStrength& LbLayer::getIorN() const
{
	return m_iorN;
}

inline const SpectralStrength& LbLayer::getIorK() const
{
	return m_iorK;
}

}// end namespace ph