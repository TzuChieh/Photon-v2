#pragma once

#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"
#include "Actor/Material/Component/RoughnessToAlphaMapping.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

class SurfaceLayerInfo final
{
public:
	SurfaceLayerInfo();

	real getRoughness() const;
	real getAlpha() const;
	math::Spectrum getIorN() const;
	math::Spectrum getIorK() const;
	real getDepth() const;
	real getG() const;
	math::Spectrum getSigmaA() const;
	math::Spectrum getSigmaS() const;

private:
	real           m_roughness;
	math::Spectrum m_iorN;
	math::Spectrum m_iorK;
	real           m_depth;
	real           m_g;
	math::Spectrum m_sigmaA;
	math::Spectrum m_sigmaS;

public:
	/*PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<SurfaceLayerInfo>)
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

inline math::Spectrum SurfaceLayerInfo::getIorN() const
{
	return m_iorN;
}

inline math::Spectrum SurfaceLayerInfo::getIorK() const
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

inline math::Spectrum SurfaceLayerInfo::getSigmaA() const
{
	return m_sigmaA;
}

inline math::Spectrum SurfaceLayerInfo::getSigmaS() const
{
	return m_sigmaS;
}

}// end namespace ph
