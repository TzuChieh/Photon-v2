#include "Engine/Actor/Material/Component/MicrosurfaceInfo.h"
#include "Engine/Actor/Material/Component/RoughnessToAlphaMapping.h"
#include "Engine/Core/SurfaceBehavior/Property/TAnisoTrowbridgeReitz.h"
#include "Engine/Core/SurfaceBehavior/Property/TIsoBeckmann.h"
#include "Engine/Core/SurfaceBehavior/Property/TIsoTrowbridgeReitz.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/World/Foundation/CookingContext.h"

#include <Common/logging.h>

#include <memory>
#include <utility>

namespace ph
{

namespace
{

PH_DEFINE_INLINE_LOG_GROUP(MicrosurfaceInfo, BSDF);

template<ERoughnessToAlpha MAPPING>
class TRoughnessToAlpha final
{
public:
	explicit TRoughnessToAlpha(TTexturedSurfaceProperty<real> roughness)
		: m_roughness(std::move(roughness))
	{}

	real operator () (const SurfaceHit& X) const
	{
		return RoughnessToAlphaMapping::map(m_roughness(X), MAPPING);
	}

private:
	TTexturedSurfaceProperty<real> m_roughness;
};

}// end namespace

template<ERoughnessToAlpha MAPPING>
std::unique_ptr<Microfacet> MicrosurfaceInfo::genTexturedMicrofacet(
	const CookingContext&        ctx,
	const EInterfaceMicrosurface microsurfaceType) const
{
	using TexturedAlpha = TRoughnessToAlpha<MAPPING>;
	using ConstantAlpha = TConstantSurfaceProperty<real>;

	if(isIsotropic())
	{
		PH_ASSERT(m_roughnessMap);
		TexturedAlpha alpha(TTexturedSurfaceProperty<real>(m_roughnessMap->genRealTexture(ctx)));

		switch(microsurfaceType)
		{
		case EInterfaceMicrosurface::TrowbridgeReitz:
			return std::make_unique<TIsoTrowbridgeReitz<TexturedAlpha>>(
				std::move(alpha),
				m_maskingShadowing);

		case EInterfaceMicrosurface::Beckmann:
			return std::make_unique<TIsoBeckmann<TexturedAlpha>>(
				std::move(alpha),
				m_maskingShadowing);

		default:
			PH_LOG(MicrosurfaceInfo, Warning,
				"type of microsurface not specified; resort to Trowbridge-Reitz (GGX)");
			return std::make_unique<TIsoTrowbridgeReitz<TexturedAlpha>>(
				std::move(alpha),
				m_maskingShadowing);
		}
	}

	// Anisotropic

	if(microsurfaceType != EInterfaceMicrosurface::TrowbridgeReitz)
	{
		PH_LOG(MicrosurfaceInfo, Warning,
			"type of microsurface not specified; resort to Trowbridge-Reitz (GGX)");
	}

	if(m_roughnessMap && m_roughnessVMap)
	{
		return std::make_unique<TAnisoTrowbridgeReitz<TexturedAlpha, TexturedAlpha>>(
			TexturedAlpha(TTexturedSurfaceProperty<real>(m_roughnessMap->genRealTexture(ctx))),
			TexturedAlpha(TTexturedSurfaceProperty<real>(m_roughnessVMap->genRealTexture(ctx))),
			m_maskingShadowing);
	}

	if(m_roughnessMap)
	{
		return std::make_unique<TAnisoTrowbridgeReitz<TexturedAlpha, ConstantAlpha>>(
			TexturedAlpha(TTexturedSurfaceProperty<real>(m_roughnessMap->genRealTexture(ctx))),
			ConstantAlpha(RoughnessToAlphaMapping::map(m_roughnessV.value_or(m_roughness), MAPPING)),
			m_maskingShadowing);
	}

	PH_ASSERT(m_roughnessVMap);
	return std::make_unique<TAnisoTrowbridgeReitz<ConstantAlpha, TexturedAlpha>>(
		ConstantAlpha(RoughnessToAlphaMapping::map(m_roughness, MAPPING)),
		TexturedAlpha(TTexturedSurfaceProperty<real>(m_roughnessVMap->genRealTexture(ctx))),
		m_maskingShadowing);
}

std::unique_ptr<Microfacet> MicrosurfaceInfo::genMicrofacet(
	const CookingContext&        ctx,
	const EInterfaceMicrosurface defaultType) const
{
	EInterfaceMicrosurface microsurfaceType = m_microsurface;
	if(microsurfaceType == EInterfaceMicrosurface::Unspecified)
	{
		microsurfaceType = defaultType;
	}

	if(m_roughnessMap || m_roughnessVMap)
	{
		switch(m_roughnessToAlpha)
		{
		case ERoughnessToAlpha::Equaled:
			return genTexturedMicrofacet<ERoughnessToAlpha::Equaled>(ctx, microsurfaceType);

		case ERoughnessToAlpha::PbrtV3:
			return genTexturedMicrofacet<ERoughnessToAlpha::PbrtV3>(ctx, microsurfaceType);

		case ERoughnessToAlpha::Squared:
		case ERoughnessToAlpha::Unspecified:
		default:
			return genTexturedMicrofacet<ERoughnessToAlpha::Squared>(ctx, microsurfaceType);
		}
	}

	// Non-mapped alpha

	if(isIsotropic())
	{
		using Alpha = TConstantSurfaceProperty<real>;
		const Alpha alpha(RoughnessToAlphaMapping::map(getIsotropicRoughness(), m_roughnessToAlpha));

		switch(microsurfaceType)
		{
		case EInterfaceMicrosurface::TrowbridgeReitz:
			return std::make_unique<TIsoTrowbridgeReitz<Alpha>>(alpha, m_maskingShadowing);

		case EInterfaceMicrosurface::Beckmann:
			return std::make_unique<TIsoBeckmann<Alpha>>(alpha, m_maskingShadowing);

		default:
			PH_LOG(MicrosurfaceInfo, Warning,
				"type of microsurface not specified; resort to Trowbridge-Reitz (GGX)");
			return std::make_unique<TIsoTrowbridgeReitz<Alpha>>(alpha, m_maskingShadowing);
		}
	}
	else
	{
		PH_ASSERT(!isIsotropic());

		if(microsurfaceType == EInterfaceMicrosurface::Beckmann)
		{
			PH_LOG(MicrosurfaceInfo, Warning,
				"anisotropic Beckmann is not supported; resort to Trowbridge-Reitz (GGX)");
		}

		using Alpha = TConstantSurfaceProperty<real>;

		const auto [roughnessU, roughnessV] = getAnisotropicUVRoughnesses();
		const Alpha alphaU(RoughnessToAlphaMapping::map(roughnessU, m_roughnessToAlpha));
		const Alpha alphaV(RoughnessToAlphaMapping::map(roughnessV, m_roughnessToAlpha));

		return std::make_unique<TAnisoTrowbridgeReitz<Alpha, Alpha>>(
			alphaU,
			alphaV,
			m_maskingShadowing);
	}
}

void MicrosurfaceInfo::setRoughnessMap(std::shared_ptr<Image> roughnessMap)
{
	m_roughnessMap = std::move(roughnessMap);
}

void MicrosurfaceInfo::setRoughnessVMap(std::shared_ptr<Image> roughnessVMap)
{
	m_roughnessVMap = std::move(roughnessVMap);
}

}// end namespace ph
