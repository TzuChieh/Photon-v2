#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Core/Quantity/Spectrum.h"
#include "Core/Texture/TTexture.h"
#include "Math/Random/TPwcDistribution2D.h"
#include "Math/TVector2.h"

#include <memory>
#include <cstddef>

namespace ph
{

/*
	A background emitter represents energy coming from effectively infinite 
	distances from the world geometries. As a surface emitter, it expects
	the associated surface primitive to satisfy the following properties:
	
	1. large enough to contain the entire scene
	2. normalized and one-to-one position <-> uvw mapping

	Associating surface primitives that do not meet these requirements may 
	results in rendering artifacts.
*/

class Primitive;

class BackgroundEmitter : public SurfaceEmitter
{
public:
	using RadianceTexture = std::shared_ptr<TTexture<Spectrum>>;

	BackgroundEmitter(
		const Primitive*                   surface,
		const RadianceTexture&             radiance,
		const math::TVector2<std::size_t>& resolution,
		real sceneBoundRadius);

	void evalEmittedRadiance(const SurfaceHit& X, Spectrum* out_radiance) const override;
	void genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const override;

	// FIXME: ray time
	void emitRay(SampleFlow& sampleFlow, Ray* out_ray, Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const override;
	real calcRadiantFluxApprox() const override;

	// HACK
	bool isBackground() const override
	{
		return true;
	}

private:
	const Primitive*               m_surface;
	RadianceTexture                m_radiance;
	math::TPwcDistribution2D<real> m_sampleDistribution;
	real                           m_radiantFluxApprox;
	real m_sceneBoundRadius;
};

}// end namespace ph
