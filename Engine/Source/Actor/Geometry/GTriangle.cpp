#include "Actor/Geometry/GTriangle.h"
#include "Core/Intersectable/PTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Intersectable/UvwMapper/UvwMapper.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Common/assertion.h"
#include "Math/math.h"
#include "Math/Geometry/TTriangle.h"

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
	m_nA  (0),
	m_nB  (0),
	m_nC  (0)
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

	const auto faceNormal = math::TTriangle<real>(m_vA, m_vB, m_vC).getFaceNormalSafe();
	triangle.setNa(m_nA.isNotZero() ? m_nA.normalize() : faceNormal);
	triangle.setNb(m_nB.isNotZero() ? m_nB.normalize() : faceNormal);
	triangle.setNc(m_nC.isNotZero() ? m_nC.normalize() : faceNormal);

	out_primitives.push_back(std::make_unique<PTriangle>(triangle));
}

std::shared_ptr<Geometry> GTriangle::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	auto tTriangle = std::make_shared<GTriangle>(*this);

	transform.transformP(m_vA, &tTriangle->m_vA);
	transform.transformP(m_vB, &tTriangle->m_vB);
	transform.transformP(m_vC, &tTriangle->m_vC);

	const auto faceNormal = math::TTriangle<real>(m_vA, m_vB, m_vC).getFaceNormalSafe();

	math::Vector3R transformedN;
	transform.transformO(m_nA, &transformedN);
	tTriangle->setNa(transformedN.isNotZero() ? transformedN.normalize() : faceNormal);
	transform.transformO(m_nB, &transformedN);
	tTriangle->setNb(transformedN.isNotZero() ? transformedN.normalize() : faceNormal);
	transform.transformO(m_nC, &transformedN);
	tTriangle->setNc(transformedN.isNotZero() ? transformedN.normalize() : faceNormal);

	return tTriangle;
}

bool GTriangle::isDegenerate() const
{
	return math::TTriangle<real>(m_vA, m_vB, m_vC).isDegenerate();
}

}// end namespace ph
