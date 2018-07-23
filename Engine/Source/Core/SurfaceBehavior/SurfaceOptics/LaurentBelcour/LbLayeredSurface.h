#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"

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
		const std::vector<real>&             alphas);
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
};

// In-header Implementations:



}// end namespace ph