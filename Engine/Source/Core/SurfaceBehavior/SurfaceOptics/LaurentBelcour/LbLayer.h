#pragma once

#include "Core/Quantity/Spectrum.h"
#include "Common/primitive_type.h"

namespace ph
{

class LbLayer
{
public:
	LbLayer();
	explicit LbLayer(const Spectrum& iorN);
	LbLayer(real alpha, const Spectrum& iorN);
	LbLayer(real alpha, const Spectrum& iorN, const Spectrum& iorK);
	LbLayer(real g, real depth, const LbLayer& previousLayer);
	LbLayer(real g, real depth, const Spectrum& sigmaA, const LbLayer& previousLayer);
	LbLayer(real g, real depth, const Spectrum& sigmaA, const Spectrum& sigmaS, const LbLayer& previousLayer);

	real getAlpha() const;
	const Spectrum& getIorN() const;
	const Spectrum& getIorK() const;
	real getDepth() const;
	real getG() const;
	const Spectrum& getSigmaA() const;
	const Spectrum& getSigmaS() const;
	bool isConductor() const;
	bool isSurface() const;
	bool isVolume() const;

private:
	// The alpha variable for GGX distribution.
	real     m_alpha;

	// Complex index of refraction n + ik.
	Spectrum m_iorN, m_iorK;

	// Thickness of the layer.
	real     m_depth;

	// The g variable in Henyey-Greenstein phase function.
	real     m_g;

	// Absorption and scattering coefficient in volume rendering.
	Spectrum m_sigmaA, m_sigmaS;
};

// In-header Implementations:

inline LbLayer::LbLayer() :
	LbLayer(Spectrum(1.0_r))
{}

inline LbLayer::LbLayer(const Spectrum& iorN) :
	LbLayer(0.0_r, iorN)
{}

inline LbLayer::LbLayer(const real alpha, const Spectrum& iorN) :
	LbLayer(alpha, iorN, Spectrum(0.0_r))
{}

inline LbLayer::LbLayer(const real alpha, const Spectrum& iorN, const Spectrum& iorK) :
	m_alpha(alpha), 
	m_iorN(iorN), m_iorK(iorK),
	m_depth(0.0_r),
	m_g(1.0_r), 
	m_sigmaA(0.0_r),
	m_sigmaS(0.0_r)
{}

inline LbLayer::LbLayer(const real g, const real depth, const LbLayer& previousLayer) : 
	LbLayer(g, depth, Spectrum(0.0_r), previousLayer)
{}

inline LbLayer::LbLayer(const real g, const real depth, const Spectrum& sigmaA, const LbLayer& previousLayer) :
	LbLayer(g, depth, sigmaA, Spectrum(0.0_r), previousLayer)
{}

inline LbLayer::LbLayer(
	const real g, 
	const real depth, 
	const Spectrum& sigmaA, 
	const Spectrum& sigmaS, 
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

inline const Spectrum& LbLayer::getIorN() const
{
	return m_iorN;
}

inline const Spectrum& LbLayer::getIorK() const
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

inline const Spectrum& LbLayer::getSigmaA() const
{
	return m_sigmaA;
}

inline const Spectrum& LbLayer::getSigmaS() const
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