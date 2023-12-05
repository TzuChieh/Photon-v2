#pragma once

#include "Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class LbLayer
{
public:
	LbLayer();
	explicit LbLayer(const math::Spectrum& iorN);
	LbLayer(real alpha, const math::Spectrum& iorN);
	LbLayer(real alpha, const math::Spectrum& iorN, const math::Spectrum& iorK);
	LbLayer(real g, real depth, const LbLayer& previousLayer);
	LbLayer(real g, real depth, const math::Spectrum& sigmaA, const LbLayer& previousLayer);
	LbLayer(real g, real depth, const math::Spectrum& sigmaA, const math::Spectrum& sigmaS, const LbLayer& previousLayer);

	real getAlpha() const;
	const math::Spectrum& getIorN() const;
	const math::Spectrum& getIorK() const;
	real getDepth() const;
	real getG() const;
	const math::Spectrum& getSigmaA() const;
	const math::Spectrum& getSigmaS() const;
	bool isConductor() const;
	bool isSurface() const;
	bool isVolume() const;

private:
	// The alpha variable for GGX distribution.
	real     m_alpha;

	// Complex index of refraction n + ik.
	math::Spectrum m_iorN, m_iorK;

	// Thickness of the layer.
	real     m_depth;

	// The g variable in Henyey-Greenstein phase function.
	real     m_g;

	// Absorption and scattering coefficient in volume rendering.
	math::Spectrum m_sigmaA, m_sigmaS;
};

// In-header Implementations:

inline LbLayer::LbLayer() :
	LbLayer(math::Spectrum(1))
{}

inline LbLayer::LbLayer(const math::Spectrum& iorN) :
	LbLayer(0.0_r, iorN)
{}

inline LbLayer::LbLayer(const real alpha, const math::Spectrum& iorN) :
	LbLayer(alpha, iorN, math::Spectrum(0))
{}

inline LbLayer::LbLayer(const real alpha, const math::Spectrum& iorN, const math::Spectrum& iorK) :
	m_alpha(alpha), 
	m_iorN(iorN), m_iorK(iorK),
	m_depth(0.0_r),
	m_g(1.0_r), 
	m_sigmaA(0.0_r),
	m_sigmaS(0.0_r)
{}

inline LbLayer::LbLayer(const real g, const real depth, const LbLayer& previousLayer) : 
	LbLayer(g, depth, math::Spectrum(0), previousLayer)
{}

inline LbLayer::LbLayer(const real g, const real depth, const math::Spectrum& sigmaA, const LbLayer& previousLayer) :
	LbLayer(g, depth, sigmaA, math::Spectrum(0), previousLayer)
{}

inline LbLayer::LbLayer(
	const real g, 
	const real depth, 
	const math::Spectrum& sigmaA,
	const math::Spectrum& sigmaS,
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

inline const math::Spectrum& LbLayer::getIorN() const
{
	return m_iorN;
}

inline const math::Spectrum& LbLayer::getIorK() const
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

inline const math::Spectrum& LbLayer::getSigmaA() const
{
	return m_sigmaA;
}

inline const math::Spectrum& LbLayer::getSigmaS() const
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