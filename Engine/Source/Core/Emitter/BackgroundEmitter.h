#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TTexture.h"
#include "Math/Random/TPwcDistribution2D.h"
#include "Math/TVector2.h"

#include <memory>
#include <cstddef>

namespace ph
{

/*
	A background emitter represents energy coming from distances significantly
	further than most of the scene geometries. As a surface emitter, it expects
	the associated surface primitive to be sufficiently large such that its 
	assumptions are true.
*/
class BackgroundEmitter : public SurfaceEmitter
{
public:
	using RadianceTexture = std::shared_ptr<TTexture<SpectralStrength>>;

	BackgroundEmitter(
		const RadianceTexture&       radiance,
		const TVector2<std::size_t>& resolution);

	void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	void genDirectSample(DirectLightSample& sample) const override;

	// FIXME: ray time
	void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const override;

private:
	RadianceTexture          m_radiance;
	TPwcDistribution2D<real> m_sampleDistribution;
};

}// end namespace ph