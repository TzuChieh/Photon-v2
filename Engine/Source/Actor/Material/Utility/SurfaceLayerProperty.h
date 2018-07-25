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

private:
	real             m_roughness;
	SpectralStrength m_iorN;
	SpectralStrength m_iorK;
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

}// end namespace ph