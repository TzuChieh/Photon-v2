#pragma once

#include "Core/Intersection/Intersectable.h"
#include "Core/Texture/TTexture.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class SurfaceHit;

/*! @brief Carve out some part of an intersectable.
This is a masking approach based on intersection routine. Another common approach to shape masking is
BSDF-based, where the mask determines whether the incoming light simply passes through (a no-op,
perfectly transmitting BSDF). There is a trade-off between these approaches: masking in intersection
routine samples the mask texture (usually a slower operation) in the tight loop of acceleration
structure traversal, while masking in BSDF will need to re-enter the acceleration structure multiple
times (such as when rendering a forest) which can also be slow.
*/
class MaskedIntersectable : public Intersectable
{
public:
	/*!
	@param intersectable The intersectable to apply the mask on.
	@param mask The mask to apply. Commonly called alpha mask or opacity mask.
	@param maxIterations For some shape, such as sphere, ray intersection must be performed
	iteratively if earlier intersections are being rejected by the mask. The default value should be
	plenty for most shapes (e.g., a sphere has at most 2 intersections for a single ray).
	*/
	MaskedIntersectable(
		const Intersectable* intersectable,
		const std::shared_ptr<TTexture<real>>& mask,
		uint8 maxIterations = 4);

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
	std::shared_ptr<TTexture<real>> m_mask;
	uint8 m_maxIterations;
};

}// end namespace ph
