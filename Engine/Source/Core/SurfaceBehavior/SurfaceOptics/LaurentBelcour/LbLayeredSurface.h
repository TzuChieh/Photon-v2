#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayer.h"

#include <vector>

namespace ph
{

/*
	Laurent Belcour's layered BSDF model.

	Reference: 
	
	Efficient Rendering of Layered Materials using an Atomic Decomposition 
	with Statistical Operators

	ACM Transactions on Graphics (proc. of SIGGRAPH 2018)

	- Project Page
	https://belcour.github.io/blog/research/2018/05/05/brdf-realtime-layered.html
*/
class LbLayeredSurface : public SurfaceOptics
{
public:
	LbLayeredSurface(
		const std::vector<SpectralStrength>& iorNs,
		const std::vector<SpectralStrength>& iorKs,
		const std::vector<real>&             alphas,
		const std::vector<real>&             depths,
		const std::vector<real>&             gs,
		const std::vector<SpectralStrength>& sigmaAs,
		const std::vector<SpectralStrength>& sigmaSs);
	~LbLayeredSurface() override;

private:
	void evalBsdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf) const override;

	void genBsdfSample(
		const SurfaceHit& X, const Vector3R& V,
		Vector3R* out_L,
		SpectralStrength* out_pdfAppliedBsdf) const override;

	void calcBsdfSamplePdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		real* out_pdfW) const override;

	std::vector<SpectralStrength> m_iorNs;
	std::vector<SpectralStrength> m_iorKs;
	std::vector<real>             m_alphas;
	std::vector<real>             m_depths;
	std::vector<real>             m_gs;
	std::vector<SpectralStrength> m_sigmaAs;
	std::vector<SpectralStrength> m_sigmaSs;

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

}// end namespace ph