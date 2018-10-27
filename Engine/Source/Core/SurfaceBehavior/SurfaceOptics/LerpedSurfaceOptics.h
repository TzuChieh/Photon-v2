#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Texture/texture_fwd.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/primitive_type.h"
#include "Core/Texture/TSampler.h"

#include <memory>

namespace ph
{

class LerpedSurfaceOptics : public SurfaceOptics
{
public:
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0, 
		const std::shared_ptr<SurfaceOptics>& optics1);
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0,
		const std::shared_ptr<SurfaceOptics>& optics1,
		real ratio);
	LerpedSurfaceOptics(
		const std::shared_ptr<SurfaceOptics>& optics0, 
		const std::shared_ptr<SurfaceOptics>& optics1,
		const std::shared_ptr<TTexture<SpectralStrength>>& ratio);

private:
	std::shared_ptr<SurfaceOptics>              m_optics0;
	std::shared_ptr<SurfaceOptics>              m_optics1;
	std::shared_ptr<TTexture<SpectralStrength>> m_ratio;
	TSampler<SpectralStrength>                  m_sampler;

	void calcBsdf(
		const BsdfEvaluation::Input& in,
		BsdfEvaluation::Output&      out,
		const SidednessAgreement&    sidedness) const override;

	void calcBsdfSample(
		const BsdfSample::Input&     in,
		BsdfSample::Output&          out,
		const SidednessAgreement&    sidedness) const override;

	void calcBsdfSamplePdfW(
		const BsdfPdfQuery::Input&   in,
		BsdfPdfQuery::Output&        out,
		const SidednessAgreement&    sidedness) const override;

	static real probabilityOfPickingOptics0(const SpectralStrength& ratio);
};

}// end namespace ph