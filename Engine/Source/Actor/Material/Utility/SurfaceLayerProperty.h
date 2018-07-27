#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"

#include <vector>

namespace ph
{

class InputPacket;

class SurfaceLayerProperty
{
public:
	SurfaceLayerProperty();
	explicit SurfaceLayerProperty(const InputPacket& packet);

	real getRoughness() const;
	real getAlpha() const;
	SpectralStrength getIorN() const;
	SpectralStrength getIorK() const;
	real getDepth() const;
	real getG() const;
	SpectralStrength getSigmaA() const;
	SpectralStrength getSigmaS() const;

private:
	real             m_roughness;
	SpectralStrength m_iorN;
	SpectralStrength m_iorK;
	real             m_depth;
	real             m_g;
	SpectralStrength m_sigmaA;
	SpectralStrength m_sigmaS;
};

// In-header Implementations:

inline real SurfaceLayerProperty::getRoughness() const
{
	return m_roughness;
}

inline real SurfaceLayerProperty::getAlpha() const
{
	return RoughnessToAlphaMapping::squared(getRoughness());
}

inline SpectralStrength SurfaceLayerProperty::getIorN() const
{
	return m_iorN;
}

inline SpectralStrength SurfaceLayerProperty::getIorK() const
{
	return m_iorK;
}

inline real SurfaceLayerProperty::getDepth() const
{
	return m_depth;
}

inline real SurfaceLayerProperty::getG() const
{
	return m_g;
}

inline SpectralStrength SurfaceLayerProperty::getSigmaA() const
{
	return m_sigmaA;
}

inline SpectralStrength SurfaceLayerProperty::getSigmaS() const
{
	return m_sigmaS;
}

}// end namespace ph