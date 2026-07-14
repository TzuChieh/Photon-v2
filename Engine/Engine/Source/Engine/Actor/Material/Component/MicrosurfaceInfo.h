#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/Material/Component/sdl_component_enums.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <memory>
#include <optional>
#include <utility>

namespace ph
{

class CookingContext;
class Microfacet;

class MicrosurfaceInfo final
{
public:
	/*!
	@param defaultType The default microsurface type to use if unspecified.
	@return The generated microfacet describing the microsurface.
	*/
	std::unique_ptr<Microfacet> genMicrofacet(
		const CookingContext& ctx,
		EInterfaceMicrosurface defaultType = EInterfaceMicrosurface::TrowbridgeReitz) const;

	bool isIsotropic() const;
	real getIsotropicRoughness() const;
	std::pair<real, real> getAnisotropicUVRoughnesses() const;

	void setRoughnessMap(std::shared_ptr<Image> roughnessMap);
	void setRoughnessVMap(std::shared_ptr<Image> roughnessVMap);

private:
	template<ERoughnessToAlpha MAPPING>
	std::unique_ptr<Microfacet> genTexturedMicrofacet(
		const CookingContext& ctx,
		EInterfaceMicrosurface microsurfaceType) const;

	EInterfaceMicrosurface m_microsurface;
	real                   m_roughness;
	std::shared_ptr<Image> m_roughnessMap;
	std::optional<real>    m_roughnessV;
	std::shared_ptr<Image> m_roughnessVMap;
	ERoughnessToAlpha      m_roughnessToAlpha;
	EMaskingShadowing      m_maskingShadowing;

public:
	PH_DEFINE_SDL_STRUCT(MicrosurfaceInfo, ztruct)
	{
		ztruct.typeName("microsurface");
		ztruct.description(
			"Describing microsurface structure of the material. "
			"For paired value/map inputs, map inputs have higher precedence.");

		TSdlEnumField<OwnerType, EInterfaceMicrosurface> microsurface("microsurface", &OwnerType::m_microsurface);
		microsurface.description("Type of the microsurface of the material.");
		microsurface.optional();
		microsurface.defaultTo(EInterfaceMicrosurface::Unspecified);
		ztruct.addField(microsurface);

		TSdlReal<OwnerType> roughness("roughness", &OwnerType::m_roughness);
		roughness.description(
			"Isotropic surface roughness in [0, 1], the material will appear "
			"to be smoother with smaller roughness value.");
		roughness.optional();
		roughness.defaultTo(0.5_r);
		ztruct.addField(roughness);

		TSdlReference<Image, OwnerType> roughnessMap("roughness-map", &OwnerType::m_roughnessMap);
		roughnessMap.description("Texture-mapped isotropic surface roughness in [0, 1].");
		roughnessMap.optional();
		ztruct.addField(roughnessMap);

		TSdlOptionalReal<OwnerType> roughnessV("roughness-v", &OwnerType::m_roughnessV);
		roughnessV.description(
			"Similar to the `roughness` parameter, but is used for anisotropic "
			"surface appearances. This value controls the V component of "
			"surface roughness. If this value is provided, the `roughness` "
			"parameter is interpreted as the U component of surface roughness.");
		ztruct.addField(roughnessV);

		TSdlReference<Image, OwnerType> roughnessVMap("roughness-v-map", &OwnerType::m_roughnessVMap);
		roughnessVMap.description(
			"Texture-mapped V component of anisotropic surface roughness in [0, 1]. "
			"If this value is provided, `roughness` or `roughness-map` is interpreted "
			"as the U component of surface roughness.");
		roughnessVMap.optional();
		ztruct.addField(roughnessVMap);

		TSdlEnumField<OwnerType, ERoughnessToAlpha> roughnessToAlpha("roughness-to-alpha", &OwnerType::m_roughnessToAlpha);
		roughnessToAlpha.description("Type of the mapping to transform roughness into alpha value.");
		roughnessToAlpha.optional();
		roughnessToAlpha.defaultTo(ERoughnessToAlpha::Squared);
		ztruct.addField(roughnessToAlpha);

		TSdlEnumField<OwnerType, EMaskingShadowing> maskingShadowing("masking-shadowing", &OwnerType::m_maskingShadowing);
		maskingShadowing.description("Type of the masking and shadowing for a microsurface.");
		maskingShadowing.optional();
		maskingShadowing.defaultTo(EMaskingShadowing::HightCorrelated);
		ztruct.addField(maskingShadowing);
	}
};

// In-header Implementations:

inline bool MicrosurfaceInfo::isIsotropic() const
{
	if(m_roughnessVMap)
	{
		return false;
	}

	if(m_roughnessMap)
	{
		return !m_roughnessV.has_value();
	}

	return !m_roughnessV.has_value() ||
	       (m_roughnessV.has_value() && m_roughness == *m_roughnessV);
}

inline real MicrosurfaceInfo::getIsotropicRoughness() const
{
	PH_ASSERT(isIsotropic());
	PH_ASSERT(!m_roughnessMap);
	PH_ASSERT(!m_roughnessVMap);

	return m_roughness;
}

inline std::pair<real, real> MicrosurfaceInfo::getAnisotropicUVRoughnesses() const
{
	PH_ASSERT(!isIsotropic());
	PH_ASSERT(!m_roughnessMap);
	PH_ASSERT(!m_roughnessVMap);
	PH_ASSERT(m_roughnessV.has_value());

	return {m_roughness, *m_roughnessV};
}

}// end namespace ph
