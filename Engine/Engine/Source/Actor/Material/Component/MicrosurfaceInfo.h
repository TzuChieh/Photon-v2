#pragma once

#include "Actor/Material/Component/sdl_component_enums.h"
#include "SDL/sdl_interface.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <memory>
#include <optional>
#include <utility>

namespace ph
{

class Microfacet;

class MicrosurfaceInfo final
{
public:
	/*!
	@param defaultType The default microsurface type to use if unspecified.
	@return The generated microfacet describing the microsurface.
	*/
	std::unique_ptr<Microfacet> genMicrofacet(
		EInterfaceMicrosurface defaultType = EInterfaceMicrosurface::TrowbridgeReitz) const;

	bool isIsotropic() const;
	real getIsotropicRoughness() const;
	std::pair<real, real> getAnisotropicUVRoughnesses() const;

private:
	EInterfaceMicrosurface m_microsurface;
	real                   m_roughness;
	std::optional<real>    m_roughnessV;
	ERoughnessToAlpha      m_roughnessToAlpha;
	EMaskingShadowing      m_maskingShadowing;

public:
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<MicrosurfaceInfo>)
	{
		StructType ztruct("microsurface");
		ztruct.description("Describing microsurface structure of the material.");

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

		TSdlOptionalReal<OwnerType> roughnessV("roughness-v", &OwnerType::m_roughnessV);
		roughnessV.description(
			"Similar to the `roughness` parameter, but is used for anisotropic "
			"surface appearances. This value controls the V component of "
			"surface roughness. If this value is provided, the `roughness` "
			"parameter is interpreted as the U component of surface roughness.");
		ztruct.addField(roughnessV);

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

		return ztruct;
	}
};

// In-header Implementations:

inline bool MicrosurfaceInfo::isIsotropic() const
{
	return !m_roughnessV.has_value() || 
	       (m_roughnessV.has_value() && m_roughness == *m_roughnessV);
}

inline real MicrosurfaceInfo::getIsotropicRoughness() const
{
	PH_ASSERT(isIsotropic());

	return m_roughness;
}

inline std::pair<real, real> MicrosurfaceInfo::getAnisotropicUVRoughnesses() const
{
	PH_ASSERT(!isIsotropic());
	PH_ASSERT(m_roughnessV.has_value());

	return {m_roughness, *m_roughnessV};
}

}// end namespace ph
