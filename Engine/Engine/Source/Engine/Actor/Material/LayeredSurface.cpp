#include "Engine/Actor/Material/LayeredSurface.h"
#include "Engine/Actor/Material/Component/RoughnessToAlphaMapping.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayerProperty.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TLbLayerProperty.h"
#include "Engine/Core/Texture/constant_textures.h"
#include "Engine/SDL/TSdl.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>

#include <memory>
#include <utility>
#include <vector>

namespace ph
{

namespace
{

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

}// end anonymous namespace

void LayeredSurface::storeCooked(
	const CookingContext& ctx,
	CookedMaterial& out_material) const
{
	using ConstantReal = TConstantSurfaceProperty<real>;
	using ConstantSpectrum = TConstantSurfaceProperty<math::Spectrum>;
	using ConstantLayer = TLbLayerProperty<
		ConstantReal,
		ConstantSpectrum,
		ConstantSpectrum,
		ConstantReal,
		ConstantReal,
		ConstantSpectrum,
		ConstantSpectrum>;

	using TexturedReal = TTexturedSurfaceProperty<real>;
	using TexturedSpectrum = TTexturedSurfaceProperty<math::Spectrum>;
	using TexturedAlpha = TRoughnessToAlpha<ERoughnessToAlpha::Squared>;
	using TexturedLayer = TLbLayerProperty<
		TexturedAlpha,
		TexturedSpectrum,
		TexturedSpectrum,
		TexturedReal,
		TexturedReal,
		TexturedSpectrum,
		TexturedSpectrum>;

	const auto genRealTexture = [&ctx](
		const std::shared_ptr<Image>& image,
		const real constant) -> std::shared_ptr<TTexture<real>>
	{
		if(image)
		{
			return image->genRealTexture(ctx);
		}

		return std::make_shared<TConstantTexture<real>>(constant);
	};

	const auto genSpectrumTexture = [&ctx](
		const std::shared_ptr<Image>& image,
		const math::Spectrum& constant) -> std::shared_ptr<TTexture<math::Spectrum>>
	{
		if(image)
		{
			return image->genColorTexture(ctx);
		}

		return std::make_shared<TConstantTexture<math::Spectrum>>(constant);
	};

	std::vector<std::shared_ptr<LbLayerProperty>> layerProperties;
	layerProperties.reserve(m_layers.size());
	for(const auto& srcLayer : m_layers)
	{
		if(srcLayer.hasAnyMap())
		{
			layerProperties.push_back(std::make_shared<TexturedLayer>(
				TexturedAlpha(TexturedReal(genRealTexture(srcLayer.getRoughnessMap(), srcLayer.getRoughness()))),
				TexturedSpectrum(genSpectrumTexture(srcLayer.getIorNMap(), srcLayer.getIorN())),
				TexturedSpectrum(genSpectrumTexture(srcLayer.getIorKMap(), srcLayer.getIorK())),
				TexturedReal(genRealTexture(srcLayer.getDepthMap(), srcLayer.getDepth())),
				TexturedReal(genRealTexture(srcLayer.getGMap(), srcLayer.getG())),
				TexturedSpectrum(genSpectrumTexture(srcLayer.getSigmaAMap(), srcLayer.getSigmaA())),
				TexturedSpectrum(genSpectrumTexture(srcLayer.getSigmaSMap(), srcLayer.getSigmaS()))));
		}
		else
		{
			layerProperties.push_back(std::make_shared<ConstantLayer>(
				ConstantReal(srcLayer.getAlpha()),
				ConstantSpectrum(srcLayer.getIorN()),
				ConstantSpectrum(srcLayer.getIorK()),
				ConstantReal(srcLayer.getDepth()),
				ConstantReal(srcLayer.getG()),
				ConstantSpectrum(srcLayer.getSigmaA()),
				ConstantSpectrum(srcLayer.getSigmaS())));
		}
	}

	out_material.surfaceOptics = ctx.getResources().makeSurfaceOptics<LbLayeredSurface>(
		std::move(layerProperties));
}

void LayeredSurface::addLayer()
{
	m_layers.push_back(TSdl<SurfaceLayerInfo>::make());
}

void LayeredSurface::setLayer(const std::size_t layerIndex, const SurfaceLayerInfo& layer)
{
	PH_ASSERT_LT(layerIndex, m_layers.size());
	m_layers[layerIndex] = layer;
}

}// end namespace ph
