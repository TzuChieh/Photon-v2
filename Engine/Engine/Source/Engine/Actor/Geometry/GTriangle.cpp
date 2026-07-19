#include "Engine/Actor/Geometry/GTriangle.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/PTriangle.h"
#include "Engine/Core/Intersection/UvwMapper/UvwMapper.h"
#include "Engine/Actor/AModel.h"
#include "Engine/Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Engine/Math/math.h"
#include "Engine/Math/Geometry/TTriangle.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <iostream>

namespace ph
{

void GTriangle::storeCooked(
	const CookingContext& ctx,
	CookedGeometry& out_geometry) const
{
	// We often have triangle that is a single point or line (one form of degenerate)
	if(isDegenerate())
	{
		return;
	}

	PTriangle triangle(m_vA, m_vB, m_vC);
	triangle.setUVWa(m_uvwA);
	triangle.setUVWb(m_uvwB);
	triangle.setUVWc(m_uvwC);

	// Renormalize supplied vertex normals; missing normals use the face normal
	if(!m_nA.isZero() || !m_nB.isZero() || !m_nC.isZero())
	{
		const math::Vector3R faceNormal = math::TTriangle<real>(m_vA, m_vB, m_vC).safeGetFaceNormal({0, 1, 0});
		triangle.setNa(m_nA.safeNormalize(faceNormal));
		triangle.setNb(m_nB.safeNormalize(faceNormal));
		triangle.setNc(m_nC.safeNormalize(faceNormal));
	}

	out_geometry.primitives.push_back(
		ctx.getResources().makeIntersectable<PTriangle>(triangle));
}

void GTriangle::storeCookedWithBakedTransform(
	const CookingContext& ctx,
	const StaticAffineTransform& transform,
	CookedGeometry& out_geometry) const
{
	GTriangle transformed = *this;

	transform.transformP(m_vA, &transformed.m_vA);
	transform.transformP(m_vB, &transformed.m_vB);
	transform.transformP(m_vC, &transformed.m_vC);

	// UVW are not affected by transformations

	// Missing normals use original triangle's face normal, so they have consistent orientation;
	// otherwise, a reflected bake would cause the face normal to flip
	if(!m_nA.isZero() || !m_nB.isZero() || !m_nC.isZero())
	{
		const math::Vector3R faceNormal = math::TTriangle<real>(m_vA, m_vB, m_vC).safeGetFaceNormal({0, 1, 0});
		transform.transformO(m_nA.isZero() ? faceNormal : m_nA, &transformed.m_nA);
		transform.transformO(m_nB.isZero() ? faceNormal : m_nB, &transformed.m_nB);
		transform.transformO(m_nC.isZero() ? faceNormal : m_nC, &transformed.m_nC);
	}

	transformed.storeCooked(ctx, out_geometry);
	out_geometry.isWindingFlipped = transform.isWindingFlipped();
}

bool GTriangle::isDegenerate() const
{
	return math::TTriangle<real>(m_vA, m_vB, m_vC).isDegenerate();
}

}// end namespace ph
