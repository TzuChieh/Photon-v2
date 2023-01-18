#include "Actor/Geometry/GTriangle.h"
#include "Core/Intersectable/PTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Common/assertion.h"
#include "Math/math.h"
#include "Math/Geometry/TTriangle.h"
#include "Actor/Basic/exceptions.h"

#include <iostream>

namespace ph
{

void GTriangle::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	PH_ASSERT(data.metadata);

	if(isDegenerate() || 
	   (!m_vA.isFinite() || !m_vB.isFinite() || !m_vC.isFinite()))
	{
		throw_formatted<CookException>(
			"improper triangle vertex detected: v-A = {}, v-B = {}, v-C = {}",
			m_vA, m_vB, m_vC);
	}

	PTriangle triangle(data.metadata, m_vA, m_vB, m_vC);
	triangle.setUVWa(m_uvwA);
	triangle.setUVWb(m_uvwB);
	triangle.setUVWc(m_uvwC);

	// Use face normal if vertex normal was not supplied
	const auto faceNormal = math::TTriangle<real>(m_vA, m_vB, m_vC).getFaceNormalSafe();
	triangle.setNa(!m_nA.isZero() ? m_nA.normalize() : faceNormal);
	triangle.setNb(!m_nB.isZero() ? m_nB.normalize() : faceNormal);
	triangle.setNc(!m_nC.isZero() ? m_nC.normalize() : faceNormal);

	out_primitives.push_back(std::make_unique<PTriangle>(triangle));
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
