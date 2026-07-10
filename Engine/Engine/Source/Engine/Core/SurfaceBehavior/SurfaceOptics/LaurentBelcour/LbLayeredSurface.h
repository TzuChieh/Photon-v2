#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayerProperty.h"

#include <Common/assertion.h>

#include <memory>
#include <vector>

namespace ph
{

/*! @brief Laurent Belcour's layered BSDF model.
This is the symmetric model as described in Section 6.2 of the paper @cite Belcour:2018:Efficient.
Laurent Belcour's project page: https://belcour.github.io/blog/research/2018/05/05/brdf-realtime-layered.html.
*/
class LbLayeredSurface : public SurfaceOptics
{
public:
	explicit LbLayeredSurface(
		std::vector<std::shared_ptr<LbLayerProperty>> layerProperties);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	void calcElementalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genElementalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcElementalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

	std::string toString() const override;

private:
	std::size_t numLayers() const;

	LbLayer getLayer(
		std::size_t layerIndex,
		const SurfaceHit& X,
		const LbLayer& previousLayer) const;

	std::vector<std::shared_ptr<LbLayerProperty>> m_layerProperties;

	static thread_local std::vector<real> sampleWeights;
	static thread_local std::vector<real> alphas;
};

// In-header Implementations:

inline std::size_t LbLayeredSurface::numLayers() const
{
	return m_layerProperties.size();
}

inline LbLayer LbLayeredSurface::getLayer(
	const std::size_t layerIndex,
	const SurfaceHit& X,
	const LbLayer& previousLayer) const
{
	PH_ASSERT(layerIndex < numLayers());

	return m_layerProperties[layerIndex]->evaluate(X, previousLayer);
}

inline std::string LbLayeredSurface::toString() const
{
	return "L.B. Layered Surface, " + SurfaceOptics::toString();
}

}// end namespace ph
