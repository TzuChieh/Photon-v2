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
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
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

	// Renormalize vertex normal; use face normal if vertex normal was not supplied
	triangle.setNa(m_nA.safeNormalize(triangle.getNa()));
	triangle.setNb(m_nB.safeNormalize(triangle.getNb()));
	triangle.setNc(m_nC.safeNormalize(triangle.getNc()));

	out_geometry.primitives.push_back(
		ctx.getResources().makeIntersectable<PTriangle>(triangle));
}

std::shared_ptr<Geometry> GTriangle::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	auto tTriangle = std::make_shared<GTriangle>(*this);

	transform.transformP(m_vA, &tTriangle->m_vA);
	transform.transformP(m_vB, &tTriangle->m_vB);
	transform.transformP(m_vC, &tTriangle->m_vC);

	// UVW are not affected by transformations

	// Only transform vertex normals if they were supplied

	if(!m_nA.isZero())
	{
		math::Vector3R tN;
		transform.transformO(m_nA, &tN);
		tTriangle->m_nA = tN;
	}

	if(!m_nB.isZero())
	{
		math::Vector3R tN;
		transform.transformO(m_nB, &tN);
		tTriangle->m_nB = tN;
	}

	if(!m_nC.isZero())
	{
		math::Vector3R tN;
		transform.transformO(m_nC, &tN);
		tTriangle->m_nC = tN;
	}

	return tTriangle;
}

bool GTriangle::isDegenerate() const
{
	return math::TTriangle<real>(m_vA, m_vB, m_vC).isDegenerate();
}

}// end namespace ph
