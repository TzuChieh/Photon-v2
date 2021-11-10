#include "Actor/Geometry/GTriangle.h"
#include "Core/Intersectable/PTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include <iostream>

namespace ph
{

GTriangle::GTriangle() :
	GTriangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0})
{}

GTriangle::GTriangle(
	const math::Vector3R& vA, 
	const math::Vector3R& vB, 
	const math::Vector3R& vC) :

	Geometry(), 

	m_vA  (vA), 
	m_vB  (vB), 
	m_vC  (vC),
	m_uvwA(0, 0, 0),
	m_uvwB(1, 0, 0),
	m_uvwC(0, 1, 0),
	m_nA  (),
	m_nB  (),
	m_nC  ()
{
	PH_ASSERT_MSG(vA.isFinite() && vB.isFinite() && vC.isFinite(), "\n"
		"vA = " + vA.toString() + "\n"
		"vB = " + vB.toString() + "\n"
		"vC = " + vC.toString() + "\n");
}

void GTriangle::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	PH_ASSERT(data.metadata);

	PTriangle triangle(data.metadata, m_vA, m_vB, m_vC);
	triangle.setUVWa(m_uvwA);
	triangle.setUVWb(m_uvwB);
	triangle.setUVWc(m_uvwC);

	// Calculates face normal. Note that the vertices may form a degenerate triangle, causing 
	// zero cross product and thus producing NaNs after being normalized. In such case an arbitrary 
	// vector will be chosen.

	math::Vector3R faceNormal;
	if(!isDegenerate())
	{
		faceNormal = m_vB.sub(m_vA).cross(m_vC.sub(m_vA)).normalizeLocal();
	}
	else
	{
		faceNormal = math::Vector3R(0, 1, 0);
	}

	triangle.setNa(m_nA ? *m_nA : faceNormal);
	triangle.setNb(m_nB ? *m_nB : faceNormal);
	triangle.setNc(m_nC ? *m_nC : faceNormal);

	out_primitives.push_back(std::make_unique<PTriangle>(triangle));
}

std::shared_ptr<Geometry> GTriangle::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	auto tTriangle = std::make_shared<GTriangle>(*this);

	transform.transformP(m_vA, &tTriangle->m_vA);
	transform.transformP(m_vB, &tTriangle->m_vB);
	transform.transformP(m_vC, &tTriangle->m_vC);

	math::Vector3R transformedN;

	if(m_nA)
	{
		transform.transformO(*m_nA, &transformedN);
		transformedN.normalizeLocal();
		tTriangle->setNa(transformedN);
	}

	if(m_nB)
	{
		transform.transformO(*m_nB, &transformedN);
		transformedN.normalizeLocal();
		tTriangle->setNb(transformedN);
	}

	if(m_nC)
	{
		transform.transformO(*m_nC, &transformedN);
		transformedN.normalizeLocal();
		tTriangle->setNc(transformedN);
	}

	return tTriangle;
}

bool GTriangle::isDegenerate() const
{
	return m_vB.sub(m_vA).cross(m_vC.sub(m_vA)).lengthSquared() == 0.0_r;
}

}// end namespace ph
