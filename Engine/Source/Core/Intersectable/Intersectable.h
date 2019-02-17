#pragma once

#include "Core/Bound/TAABB3D.h"

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
a given ray is intersecting a triangle for the rest of the system to work.
Remember that we can also model the table and mug using other shapes such as
quads, they should support the same operation set as triangles. Photon supports
many kinds of *object* that can be intersected by rays, such as just-mentioned
triangles and quads, and they are named after their capability "intersectable".
*/
class Intersectable
{
public:
	virtual ~Intersectable();

	/*! @brief Determine whether a given ray hits the object.

	Checks whether the specified ray intersects this intersectable. If there is
	an intersection, true is returned and a brief intersection report is stored
	inside the probe. If there is no intersection, false is returned and the
	state of the probe is undefined.
	*/
	virtual bool isIntersecting(const Ray& ray, HitProbe& probe) const = 0;

	/*! @brief Calculates properties of a hit, such as coordinates and normal.

	This method calculates a detailed description of the intersection from the
	ray and probe used for calling isIntersecting() (if an intersection is
	found). The process of calculating intersection detail will destroy the
	input probe.
	*/
	virtual void calcIntersectionDetail(const Ray& ray, HitProbe& probe,
	                                    HitDetail* out_detail) const = 0;

	/*! @brief Calculates Axis-Aligned Bounding Box (AABB) of itself.
	*/
	virtual void calcAABB(AABB3D* out_aabb) const = 0;

	/*! @brief Determines whether this object blocks the ray.

	If greater performance is desired, you can override the default
	implementation which simply calls isIntersecting(const Ray&, HitProbe&)
	const to do the job.
	*/
	virtual bool isIntersecting(const Ray& ray) const;

	/*! @brief Conservatively checks whether this object overlaps a volume.

	By conservative, it means **true can be returned even though the object does
	not overlap the volume**; but if it actually does, **true must be
	returned**. The default implementation performs conservative intersecting
	test using the AABB calculated by calcAABB(). Although false-positives are
	allowed for this method, providing an implementation with higher accuracy is
	benefitial for many algorithms used by the renderer.
	*/
	virtual bool isIntersectingVolumeConservative(const AABB3D& volume) const;
};

}// end namespace ph