#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"

#include <Common/compiler.h>
#include <Common/primitive_type.h>

namespace ph
{

class SurfaceHit;

class MaterialInterfaceMask final
{
public:
	real operator () (const SurfaceHit& X) const;
};

/*! @brief Carve out some part of an intersectable.
This is a masking approach based on intersection routine. Another common approach to shape masking is
BSDF-based, where the mask determines whether the incoming light simply passes through (a no-op,
perfectly transmitting BSDF). There is a trade-off between these approaches: masking in intersection
routine samples the mask texture (usually a slower operation) in the tight loop of acceleration
structure traversal, while masking in BSDF will need to re-enter the acceleration structure multiple
times (such as when rendering a forest) which can also be slow.
*/
template<typename Mask>
class TMaskedIntersectable : public Intersectable
{
	static_assert(CSurfaceProperty<Mask, real>,
		"'Mask' must accept 'SurfaceHit' and return a real-convertible value.");

public:
	/*!
	@param intersectable The intersectable to apply the mask on.
	@param mask The mask to apply. Commonly called alpha mask or opacity mask.
	@param maxIterations For some shapes, such as a sphere, ray intersection must be performed
	iteratively if earlier intersections are being rejected by the mask. A sphere has at most 2
	intersections for a single ray, while a mesh may contain arbitrarily many layers. The default
	value of 1024 should be plenty even for trees.
	*/
	TMaskedIntersectable(
		const Intersectable* intersectable,
		Mask mask,
		uint32 maxIterations = 1024);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override;

	void calcHitDetail(
		const Ray& ray,
		HitProbe& probe,
		HitDetail* out_detail) const override;

	math::AABB3D calcAABB() const override;

private:
	bool isOnMask(const SurfaceHit& X) const;

	const Intersectable* m_intersectable;

	[[PH_NO_UNIQUE_ADDRESS]]
	Mask m_mask;

	uint32 m_maxIterations;
};

using MaskedIntersectable = TMaskedIntersectable<TTexturedSurfaceProperty<real>>;
using MaterialMaskedIntersectable = TMaskedIntersectable<MaterialInterfaceMask>;

}// end namespace ph

#include "Engine/Core/Intersection/TMaskedIntersectable.ipp"
