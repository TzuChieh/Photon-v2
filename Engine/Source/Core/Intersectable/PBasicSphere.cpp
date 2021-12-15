#include "Core/Intersectable/PBasicSphere.h"
#include "Common/assertion.h"
#include "Math/constant.h"
#include "Math/Geometry/TAABB3D.h"
#include "Math/math.h"
#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Math/TMatrix2.h"
#include "Core/Sample/PositionSample.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/Geometry/TSphere.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ph
{

PBasicSphere::PBasicSphere(const PrimitiveMetadata* const metadata, const real radius) :
	Primitive(metadata),
	m_radius(radius), m_rcpRadius(radius > 0.0_r ? 1.0_r / radius : 0.0_r)
{
	PH_ASSERT_GE(radius, 0.0_r);
}

bool PBasicSphere::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	real hitT;
	if(!math::TSphere(m_radius).isIntersecting(ray.getSegment(), &hitT))
	{
		return false;
	}

	probe.pushBaseHit(this, hitT);
	return true;
}

// Intersection test for solid box and hollow sphere.
// Reference: Jim Arvo's algorithm in Graphics Gems 2
bool PBasicSphere::mayIntersectVolume(const math::AABB3D& volume) const
{
	const real radius2 = math::squared(m_radius);

	// These variables are gonna store minimum and maximum squared distances 
	// from the sphere's center to the AABB volume.
	real minDist2 = 0.0_r;
	real maxDist2 = 0.0_r;

	real a, b;

	a = math::squared(volume.getMinVertex().x());
	b = math::squared(volume.getMaxVertex().x());
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().x()) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().x()) minDist2 += b;

	a = math::squared(volume.getMinVertex().y());
	b = math::squared(volume.getMaxVertex().y());
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().y()) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().y()) minDist2 += b;

	a = math::squared(volume.getMinVertex().z());
	b = math::squared(volume.getMaxVertex().z());
	maxDist2 += std::max(a, b);
	if     (0.0_r < volume.getMinVertex().z()) minDist2 += a;
	else if(0.0_r > volume.getMaxVertex().z()) minDist2 += b;

	return minDist2 <= radius2 && radius2 <= maxDist2;
}

math::AABB3D PBasicSphere::calcAABB() const
{
	return math::AABB3D(
		math::Vector3R(-m_radius, -m_radius, -m_radius),
		math::Vector3R( m_radius,  m_radius,  m_radius)).
		expand(math::Vector3R(0.0001_r * m_radius));
}

real PBasicSphere::calcPositionSamplePdfA(const math::Vector3R& position) const
{
	return 1.0_r / PBasicSphere::calcExtendedArea();
}

void PBasicSphere::genPositionSample(SampleFlow& sampleFlow, PositionSample* const out_sample) const
{
	PH_ASSERT(out_sample);
	PH_ASSERT(m_metadata);

	out_sample->normal = math::TSphere<real>::makeUnit().sampleToSurfaceArchimedes(
		sampleFlow.flow2D());

	out_sample->position = out_sample->normal.mul(m_radius);

	// FIXME: able to specify mapper channel
	const UvwMapper* mapper = m_metadata->getDefaultChannel().getMapper();
	PH_ASSERT(mapper);
	mapper->positionToUvw(out_sample->position, &out_sample->uvw);

	// FIXME: assumed uniform PDF
	out_sample->pdf = PBasicSphere::calcPositionSamplePdfA(out_sample->position);
}

real PBasicSphere::calcExtendedArea() const
{
	return math::TSphere(m_radius).getArea();
}

}// end namespace ph
