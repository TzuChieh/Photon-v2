#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Actor/Material/Component/RoughnessToAlphaMapping.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"

#include <Common/primitive_type.h>

#include <memory>

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

	const std::shared_ptr<Image>& getRoughnessMap() const;
	const std::shared_ptr<Image>& getIorNMap() const;
	const std::shared_ptr<Image>& getIorKMap() const;
	const std::shared_ptr<Image>& getDepthMap() const;
	const std::shared_ptr<Image>& getGMap() const;
	const std::shared_ptr<Image>& getSigmaAMap() const;
	const std::shared_ptr<Image>& getSigmaSMap() const;

	bool hasAnyMap() const;

	void setRoughnessMap(std::shared_ptr<Image> roughnessMap);
	void setIorNMap(std::shared_ptr<Image> iorNMap);
	void setIorKMap(std::shared_ptr<Image> iorKMap);
	void setDepthMap(std::shared_ptr<Image> depthMap);
	void setGMap(std::shared_ptr<Image> gMap);
	void setSigmaAMap(std::shared_ptr<Image> sigmaAMap);
	void setSigmaSMap(std::shared_ptr<Image> sigmaSMap);

private:
	real                   m_roughness;
	std::shared_ptr<Image> m_roughnessMap;
	math::Spectrum         m_iorN;
	std::shared_ptr<Image> m_iorNMap;
	math::Spectrum         m_iorK;
	std::shared_ptr<Image> m_iorKMap;
	real                   m_depth;
	std::shared_ptr<Image> m_depthMap;
	real                   m_g;
	std::shared_ptr<Image> m_gMap;
	math::Spectrum         m_sigmaA;
	std::shared_ptr<Image> m_sigmaAMap;
	math::Spectrum         m_sigmaS;
	std::shared_ptr<Image> m_sigmaSMap;

public:
	PH_DEFINE_SDL_STRUCT(SurfaceLayerInfo, ztruct)
	{
		ztruct.typeName("surface-layer-info");
		ztruct.description(
			"Data for layered material construction. For paired value/map inputs, "
			"map inputs have higher precedence.");

		TSdlReal<OwnerType> roughness("roughness", &OwnerType::m_roughness);
		roughness.description("Roughness of the layer.");
		roughness.defaultTo(0);
		roughness.optional();
		ztruct.addField(roughness);

		TSdlReference<Image, OwnerType> roughnessMap("roughness-map", &OwnerType::m_roughnessMap);
		roughnessMap.description("Texture-mapped roughness of the layer.");
		roughnessMap.optional();
		ztruct.addField(roughnessMap);

		TSdlSpectrum<OwnerType> iorN("ior-n", math::EColorUsage::Raw, &OwnerType::m_iorN);
		iorN.description("The real part of the layer's index of refraction.");
		iorN.defaultTo(math::Spectrum(1));
		iorN.optional();
		ztruct.addField(iorN);

		TSdlReference<Image, OwnerType> iorNMap("ior-n-map", &OwnerType::m_iorNMap);
		iorNMap.description("Texture-mapped real part of the layer's index of refraction.");
		iorNMap.optional();
		ztruct.addField(iorNMap);

		TSdlSpectrum<OwnerType> iorK("ior-k", math::EColorUsage::Raw, &OwnerType::m_iorK);
		iorK.description("The imaginary part of the layer's index of refraction.");
		iorK.defaultTo(math::Spectrum(0));
		iorK.optional();
		ztruct.addField(iorK);

		TSdlReference<Image, OwnerType> iorKMap("ior-k-map", &OwnerType::m_iorKMap);
		iorKMap.description("Texture-mapped imaginary part of the layer's index of refraction.");
		iorKMap.optional();
		ztruct.addField(iorKMap);

		TSdlReal<OwnerType> depth("depth", &OwnerType::m_depth);
		depth.description("Thickness of the layer.");
		depth.defaultTo(0);
		depth.optional();
		ztruct.addField(depth);

		TSdlReference<Image, OwnerType> depthMap("depth-map", &OwnerType::m_depthMap);
		depthMap.description("Texture-mapped thickness of the layer.");
		depthMap.optional();
		ztruct.addField(depthMap);

		TSdlReal<OwnerType> g("g", &OwnerType::m_g);
		g.description("The g variable in Henyey-Greenstein phase function.");
		g.defaultTo(1);
		g.optional();
		ztruct.addField(g);

		TSdlReference<Image, OwnerType> gMap("g-map", &OwnerType::m_gMap);
		gMap.description("Texture-mapped g variable in Henyey-Greenstein phase function.");
		gMap.optional();
		ztruct.addField(gMap);

		TSdlSpectrum<OwnerType> sigmaA("sigma-a", math::EColorUsage::Raw, &OwnerType::m_sigmaA);
		sigmaA.description("The volume absorption coefficient.");
		sigmaA.defaultTo(math::Spectrum(0));
		sigmaA.optional();
		ztruct.addField(sigmaA);

		TSdlReference<Image, OwnerType> sigmaAMap("sigma-a-map", &OwnerType::m_sigmaAMap);
		sigmaAMap.description("Texture-mapped volume absorption coefficient.");
		sigmaAMap.optional();
		ztruct.addField(sigmaAMap);

		TSdlSpectrum<OwnerType> sigmaS("sigma-s", math::EColorUsage::Raw, &OwnerType::m_sigmaS);
		sigmaS.description("The volume scattering coefficient.");
		sigmaS.defaultTo(math::Spectrum(0));
		sigmaS.optional();
		ztruct.addField(sigmaS);

		TSdlReference<Image, OwnerType> sigmaSMap("sigma-s-map", &OwnerType::m_sigmaSMap);
		sigmaSMap.description("Texture-mapped volume scattering coefficient.");
		sigmaSMap.optional();
		ztruct.addField(sigmaSMap);
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

inline const std::shared_ptr<Image>& SurfaceLayerInfo::getRoughnessMap() const
{
	return m_roughnessMap;
}

inline const std::shared_ptr<Image>& SurfaceLayerInfo::getIorNMap() const
{
	return m_iorNMap;
}

inline const std::shared_ptr<Image>& SurfaceLayerInfo::getIorKMap() const
{
	return m_iorKMap;
}

inline const std::shared_ptr<Image>& SurfaceLayerInfo::getDepthMap() const
{
	return m_depthMap;
}

inline const std::shared_ptr<Image>& SurfaceLayerInfo::getGMap() const
{
	return m_gMap;
}

inline const std::shared_ptr<Image>& SurfaceLayerInfo::getSigmaAMap() const
{
	return m_sigmaAMap;
}

inline const std::shared_ptr<Image>& SurfaceLayerInfo::getSigmaSMap() const
{
	return m_sigmaSMap;
}

inline bool SurfaceLayerInfo::hasAnyMap() const
{
	return m_roughnessMap ||
	       m_iorNMap ||
	       m_iorKMap ||
	       m_depthMap ||
	       m_gMap ||
	       m_sigmaAMap ||
	       m_sigmaSMap;
}

}// end namespace ph
