#pragma once

#include "Math/Geometry/TAABB3D.h"

namespace ph
{

class Ray;
class HitProbe;
class HitDetail;

/*! @brief An object in the scene that a ray can intersect with.

To construct a scene, we must populate it with some *objects*. Imagining you are
beside a table with a mug on it, how would you describe the shape of those two
objects? Specifically, how to represent them digitally in a computer? One way to
do this is to model them using many triangles or quads. Take a triangle for
example, in a renderer like Photon, simply store the three vertices of it is not
enough: we need to support opearations on the stored data for it to be useful,
this is what intersectables are meant for.

The most common operation is **ray intersection test**. We need to know whether
a given ray is intersecting something, such as a triangle, for the rest of the
system to work. Remember that we can also model the table and mug using other
shapes such as quads, they should support the same operation set as triangles.
Photon supports many kinds of *object* that can be intersected by rays, such
as just-mentioned triangles and quads, and these objects are named after their
capability: "intersectable".
*/
class Intersectable
{
public:
	virtual ~Intersectable() = default;

	/*! @brief Determine whether a given ray hits the object.

	Checks whether the specified ray intersects this intersectable. If there is
	an intersection, true is returned and a brief hit report is stored
	inside the probe. If there is no intersection, false is returned and the
	state of the probe is undefined. `ray` and `probe` can be used for obtaining
	hit detail if an intersection is found.

	@note Generates hit event (with `ray` and `probe`).
	*/
	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const = 0;

	/*! @brief Calculates properties of a hit, such as coordinates and normal.

	This method calculates a detailed description of a hit from the ray and
	probe involved in a hit event. For example, the ray and probe used for
	calling isIntersecting() can be the inputs of this method (if an
	intersection is found). The process of calculating intersection detail will
	destroy the input probe.

	@param out_detail Stores the calculated details. This method calculates the
	essential details only. Some information such as coordinate bases will only
	be available if specifically requested afterwards (for an example, see
	`HitDetail::computeBases()`).

	@note See `Primitive` for more methods that can generate a hit event.
	*/
	virtual void calcHitDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const = 0;

	/*! @brief Calculates Axis-Aligned Bounding Box (AABB) of itself.
	*/
	virtual math::AABB3D calcAABB() const = 0;

	/*! @brief Determines whether this object blocks the ray.

	If greater performance is desired, you can override the default implementation which 
	simply calls isIntersecting(const Ray&, HitProbe&) const to do the job. The test generally
	considers the underlying shape as **hollow** (for closed shape), e.g., a sphere is not occluding
	a line segment inside the sphere.
	*/
	virtual bool isOccluding(const Ray& ray) const;

	/*! @brief Conservatively checks whether this object overlaps a volume.

	By conservative, it means **true can be returned even though the object does not overlap
	the volume**; but if it actually does, **true must be returned**. The default
	implementation performs conservative intersecting test using the AABB calculated by
	calcAABB(). Although false-positives are allowed for this method, providing an implementation 
	with higher accuracy is benefitial for many algorithms used by the renderer. The test generally
	considers the underlying shape as **hollow** (for closed shape), while the volume is **solid**.
	*/
	virtual bool mayOverlapVolume(const math::AABB3D& volume) const;
};

}// end namespace ph
