#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Math/Color/Spectrum.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayer.h"

#include <Common/primitive_type.h>

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
	LbLayeredSurface(
		const std::vector<math::Spectrum>& iorNs,
		const std::vector<math::Spectrum>& iorKs,
		const std::vector<real>&           alphas,
		const std::vector<real>&           depths,
		const std::vector<real>&           gs,
		const std::vector<math::Spectrum>& sigmaAs,
		const std::vector<math::Spectrum>& sigmaSs);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	std::string toString() const override;

private:
	void calcBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

	std::vector<math::Spectrum> m_iorNs;
	std::vector<math::Spectrum> m_iorKs;
	std::vector<real>           m_alphas;
	std::vector<real>           m_depths;
	std::vector<real>           m_gs;
	std::vector<math::Spectrum> m_sigmaAs;
	std::vector<math::Spectrum> m_sigmaSs;

	static thread_local std::vector<real> sampleWeights;
	static thread_local std::vector<real> alphas;

	std::size_t numLayers() const;
	LbLayer getLayer(std::size_t layerIndex, const LbLayer& previousLayer) const;
};

// In-header Implementations:

inline std::size_t LbLayeredSurface::numLayers() const
{
	return m_alphas.size();
}

inline std::string LbLayeredSurface::toString() const
{
	return "L.B. Layered Surface, " + SurfaceOptics::toString();
}

}// end namespace ph