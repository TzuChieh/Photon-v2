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
	bool isConductor() const;

private:
	real             m_alpha;
	SpectralStrength m_iorN, m_iorK;
};

// In-header Implementations:

inline LbLayer::LbLayer(const SpectralStrength& iorN) : 
	LbLayer(0, iorN)
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

inline bool LbLayer::isConductor() const
{
	return m_iorK.avg() > 0.0_r;
}

}// end namespace ph