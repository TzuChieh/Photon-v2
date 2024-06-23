#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TTexture.h"
#include "Math/Random/TPwcDistribution2D.h"
#include "Math/TVector2.h"

#include <memory>
#include <cstddef>

namespace ph
{

class Primitive;

/*! @brief Models energy coming from background.

A latitude-longitude environment emitter represents energy coming from 
effectively infinite distances from the world geometries. This emitter
takes only radiance functions in latitude-longitude format, i.e., with
textures parameterized by normalized spherical coordinates (with north
pole being 1).
*/
class LatLongEnvEmitter : public SurfaceEmitter
{
public:
	using RadianceTexture = std::shared_ptr<TTexture<math::Spectrum>>;

	LatLongEnvEmitter(
		const Primitive*         envSurface,
		const RadianceTexture&   radiance,
		const math::Vector2S&    resolution,
		EmitterFeatureSet        featureSet = defaultFeatureSet);

	void evalEmittedEnergy(const SurfaceHit& X, math::Spectrum* out_energy) const override;

	void genDirectSample(
		DirectEnergySampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override;

	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	real calcRadiantFluxApprox() const override;

	void setFrontFaceEmit() override;
	void setBackFaceEmit() override;

private:
	const Primitive*               m_envSurface;
	RadianceTexture                m_radiance;
	math::TPwcDistribution2D<real> m_sampleDistribution;
	real                           m_radiantFluxApprox;
};

}// end namespace ph
