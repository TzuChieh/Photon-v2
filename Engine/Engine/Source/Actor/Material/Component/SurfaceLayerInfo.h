#pragma once

#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"
#include "Actor/Material/Component/RoughnessToAlphaMapping.h"
#include "SDL/sdl_interface.h"
#include "Actor/SDLExtension/TSdlSpectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class SurfaceLayerInfo final
{
public:
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
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<SurfaceLayerInfo>)
	{
		StructType ztruct("surface-layer-info");
		ztruct.description("Data for layered material construction.");

		TSdlReal<OwnerType> roughness("roughness", &OwnerType::m_roughness);
		roughness.description("Roughness of the layer.");
		roughness.defaultTo(0);
		roughness.optional();
		ztruct.addField(roughness);

		TSdlSpectrum<OwnerType> iorN("ior-n", math::EColorUsage::Raw, &OwnerType::m_iorN);
		iorN.description("The real part of the layer's index of refraction.");
		iorN.defaultTo(math::Spectrum(1));
		iorN.optional();
		ztruct.addField(iorN);

		TSdlSpectrum<OwnerType> iorK("ior-k", math::EColorUsage::Raw, &OwnerType::m_iorK);
		iorK.description("The imaginary part of the layer's index of refraction.");
		iorK.defaultTo(math::Spectrum(0));
		iorK.optional();
		ztruct.addField(iorK);

		TSdlReal<OwnerType> depth("depth", &OwnerType::m_depth);
		depth.description("Thickness of the layer.");
		depth.defaultTo(0);
		depth.optional();
		ztruct.addField(depth);

		TSdlReal<OwnerType> g("g", &OwnerType::m_g);
		g.description("The g variable in Henyey-Greenstein phase function.");
		g.defaultTo(1);
		g.optional();
		ztruct.addField(g);

		TSdlSpectrum<OwnerType> sigmaA("sigma-a", math::EColorUsage::Raw, &OwnerType::m_sigmaA);
		sigmaA.description("The volume absorption coefficient.");
		sigmaA.defaultTo(math::Spectrum(0));
		sigmaA.optional();
		ztruct.addField(sigmaA);

		TSdlSpectrum<OwnerType> sigmaS("sigma-s", math::EColorUsage::Raw, &OwnerType::m_sigmaS);
		sigmaS.description("The volume scattering coefficient.");
		sigmaS.defaultTo(math::Spectrum(0));
		sigmaS.optional();
		ztruct.addField(sigmaS);

		return ztruct;
	}
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
