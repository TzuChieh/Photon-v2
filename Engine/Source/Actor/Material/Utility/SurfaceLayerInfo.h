#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"

#include <vector>

namespace ph
{

class InputPacket;

class SurfaceLayerInfo final
{
public:
	SurfaceLayerInfo();
	explicit SurfaceLayerInfo(const InputPacket& packet);

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

	static SpectralStrength loadIorN(const InputPacket& packet, const SpectralStrength& defaultIorN);
	static SpectralStrength loadIorK(const InputPacket& packet, const SpectralStrength& defaultIorK);
};

// In-header Implementations:

inline real SurfaceLayerInfo::getRoughness() const
{
	return m_roughness;
}

inline real SurfaceLayerInfo::getAlpha() const
{
	return RoughnessToAlphaMapping::squared(getRoughness());
}

inline SpectralStrength SurfaceLayerInfo::getIorN() const
{
	return m_iorN;
}

inline SpectralStrength SurfaceLayerInfo::getIorK() const
{
	return m_iorK;
}

inline real SurfaceLayerInfo::getDepth() const
{
	return m_depth;
}

inline real SurfaceLayerInfo::getG() const
{
	return m_g;
}

inline SpectralStrength SurfaceLayerInfo::getSigmaA() const
{
	return m_sigmaA;
}

inline SpectralStrength SurfaceLayerInfo::getSigmaS() const
{
	return m_sigmaS;
}

}// end namespace ph
