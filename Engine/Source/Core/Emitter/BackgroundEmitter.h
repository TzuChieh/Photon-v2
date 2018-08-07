#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TTexture.h"
#include "Math/Random/TPwcDistribution2D.h"
#include "Math/TVector2.h"
#include "Core/Bound/AABB3D.h"

#include <memory>
#include <cstddef>

namespace ph
{

/*
	A background emitter represents energy coming from effectively infinite 
	distances from the world geometries. As a surface emitter, it expects
	the associated surface primitive to satisfy the following properties:
	
	1. sufficiently large (at least twice the size of the scene's bounding 
	   sphere)
	2. surface parameterization should depend on directions only

	Associating surface primitives that do not meet these requirements may 
	results in rendering artifacts.
*/

class Primitive;

class BackgroundEmitter : public SurfaceEmitter
{
public:
	using RadianceTexture = std::shared_ptr<TTexture<SpectralStrength>>;

	BackgroundEmitter(
		const Primitive*             surface,
		const RadianceTexture&       radiance,
		const TVector2<std::size_t>& resolution,
		const AABB3D&                worldBound);

	void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	void genDirectSample(DirectLightSample& sample) const override;

	// FIXME: ray time
	void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;

	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const override;

private:
	const Primitive*         m_surface;
	RadianceTexture          m_radiance;
	TPwcDistribution2D<real> m_sampleDistribution;
	AABB3D                   m_worldBound;
};

}// end namespace ph