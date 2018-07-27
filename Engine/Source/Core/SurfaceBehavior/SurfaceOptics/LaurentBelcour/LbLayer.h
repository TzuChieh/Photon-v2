#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"

namespace ph
{

class LbLayer
{
public:
	LbLayer();
	explicit LbLayer(const SpectralStrength& iorN);
	LbLayer(real alpha, const SpectralStrength& iorN);
	LbLayer(real alpha, const SpectralStrength& iorN, const SpectralStrength& iorK);
	LbLayer(real g, real depth, const LbLayer& previousLayer);
	LbLayer(real g, real depth, const SpectralStrength& sigmaA, const LbLayer& previousLayer);
	LbLayer(real g, real depth, const SpectralStrength& sigmaA, const SpectralStrength& sigmaS, const LbLayer& previousLayer);

	real getAlpha() const;
	const SpectralStrength& getIorN() const;
	const SpectralStrength& getIorK() const;
	real getDepth() const;
	real getG() const;
	const SpectralStrength& getSigmaA() const;
	const SpectralStrength& getSigmaS() const;
	bool isConductor() const;
	bool isSurface() const;
	bool isVolume() const;

private:
	real             m_alpha;
	SpectralStrength m_iorN, m_iorK;
	real             m_depth;
	real             m_g;
	SpectralStrength m_sigmaA, m_sigmaS;
};

// In-header Implementations:

inline LbLayer::LbLayer() :
	LbLayer(SpectralStrength(1.0_r))
{}

inline LbLayer::LbLayer(const SpectralStrength& iorN) : 
	LbLayer(0.0_r, iorN)
{}

inline LbLayer::LbLayer(const real alpha, const SpectralStrength& iorN) :
	LbLayer(alpha, iorN, SpectralStrength(0.0_r))
{}

inline LbLayer::LbLayer(const real alpha, const SpectralStrength& iorN, const SpectralStrength& iorK) :
	m_alpha(alpha), 
	m_iorN(iorN), m_iorK(iorK),
	m_depth(0.0_r),
	m_g(1.0_r), 
	m_sigmaA(0.0_r),
	m_sigmaS(0.0_r)
{}

inline LbLayer::LbLayer(const real g, const real depth, const LbLayer& previousLayer) : 
	LbLayer(g, depth, SpectralStrength(0.0_r), previousLayer)
{}

inline LbLayer::LbLayer(const real g, const real depth, const SpectralStrength& sigmaA, const LbLayer& previousLayer) :
	LbLayer(g, depth, sigmaA, SpectralStrength(0.0_r), previousLayer)
{}

inline LbLayer::LbLayer(
	const real g, 
	const real depth, 
	const SpectralStrength& sigmaA, 
	const SpectralStrength& sigmaS, 
	const LbLayer& previousLayer) :

	m_alpha(previousLayer.getAlpha()),
	m_iorN(previousLayer.getIorN()), m_iorK(previousLayer.getIorK()),
	m_depth(depth),
	m_g(g),
	m_sigmaA(sigmaA),
	m_sigmaS(sigmaS)
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

inline real LbLayer::getDepth() const
{
	return m_depth;
}

inline real LbLayer::getG() const
{
	return m_g;
}

inline const SpectralStrength& LbLayer::getSigmaA() const
{
	return m_sigmaA;
}

inline const SpectralStrength& LbLayer::getSigmaS() const
{
	return m_sigmaS;
}

inline bool LbLayer::isConductor() const
{
	return m_iorK.avg() > 0.0_r;
}

inline bool LbLayer::isSurface() const
{
	return m_depth == 0.0_r;
}

inline bool LbLayer::isVolume() const
{
	return m_depth > 0.0_r;
}

}// end namespace ph