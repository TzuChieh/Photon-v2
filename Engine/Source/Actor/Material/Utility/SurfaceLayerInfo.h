#pragma once

#include "Core/Quantity/Spectrum.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Actor/Material/Utility/RoughnessToAlphaMapping.h"
#include "Actor/SDLExtension/sdl_interface_extended.h"

#include <vector>

namespace ph
{

class SurfaceLayerInfo final
{
public:
	SurfaceLayerInfo();

	real getRoughness() const;
	real getAlpha() const;
	Spectrum getIorN() const;
	Spectrum getIorK() const;
	real getDepth() const;
	real getG() const;
	Spectrum getSigmaA() const;
	Spectrum getSigmaS() const;

private:
	real     m_roughness;
	Spectrum m_iorN;
	Spectrum m_iorK;
	real     m_depth;
	real     m_g;
	Spectrum m_sigmaA;
	Spectrum m_sigmaS;

public:
	/*PH_DEFINE_SDL_STRUCT(TOwnerSdlStruct<SurfaceLayerInfo>)
	{
		StructType ztruct("surface-layer-info");
		ztruct.setDescription("Information for layered material construction.");

		TSdlReal<StructType> roughness("roughness", &StructType::m_roughness);
		roughness.description("Roughness of the layer.");
		roughness.defaultTo(0);
		ztruct.addField(roughness);

		return ztruct;
	}*/
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

inline Spectrum SurfaceLayerInfo::getIorN() const
{
	return m_iorN;
}

inline Spectrum SurfaceLayerInfo::getIorK() const
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

inline Spectrum SurfaceLayerInfo::getSigmaA() const
{
	return m_sigmaA;
}

inline Spectrum SurfaceLayerInfo::getSigmaS() const
{
	return m_sigmaS;
}

}// end namespace ph
