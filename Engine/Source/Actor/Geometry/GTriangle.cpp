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

GTriangle::GTriangle(const math::Vector3R& vA, const math::Vector3R& vB, const math::Vector3R& vC) :
	Geometry(), 
	m_vA(vA), m_vB(vB), m_vC(vC)
{
	PH_ASSERT_MSG(vA.isFinite() && vB.isFinite() && vC.isFinite(), "\n"
		"vA = " + vA.toString() + "\n"
		"vB = " + vB.toString() + "\n"
		"vC = " + vC.toString() + "\n");

	// Calculates face normal. Note that the vertices may form a degenerate
	// triangle, causing zero cross product and thus producing NaNs after
	// being normalized. In such case an arbitrary vector will be chosen.
	math::Vector3R faceNormal;
	if(!isDegenerate())
	{
		faceNormal = vB.sub(vA).cross(vC.sub(vA)).normalizeLocal();
	}
	else
	{
		faceNormal = math::Vector3R(0, 1, 0);
	}

	m_nA = faceNormal;
	m_nB = faceNormal;
	m_nC = faceNormal;

	m_uvwA.set(0, 0, 0);
	m_uvwB.set(1, 0, 0);
	m_uvwC.set(0, 1, 0);
}

//GTriangle::GTriangle(const InputPacket& packet) : 
//	Geometry(packet)
//{
//	const DataTreatment requiredDT(EDataImportance::REQUIRED, "GTriangle needs vA, vB, vC, nA, nB, nC, uvwA, uvwB, uvwC specified");
//
//	m_vA   = packet.getVector3r("vA",   Vector3R(0), requiredDT);
//	m_vB   = packet.getVector3r("vB",   Vector3R(0), requiredDT);
//	m_vC   = packet.getVector3r("vC",   Vector3R(0), requiredDT);
//	m_nA   = packet.getVector3r("nA",   Vector3R(0), requiredDT);
//	m_nB   = packet.getVector3r("nB",   Vector3R(0), requiredDT);
//	m_nC   = packet.getVector3r("nC",   Vector3R(0), requiredDT);
//	m_uvwA = packet.getVector3r("uvwA", Vector3R(0), requiredDT);
//	m_uvwB = packet.getVector3r("uvwB", Vector3R(0), requiredDT);
//	m_uvwC = packet.getVector3r("uvwC", Vector3R(0), requiredDT);
//}

void GTriangle::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GTriangle::genPrimitive(), " 
		          << "no PrimitiveMetadata" << std::endl;
		return;
	}

	PTriangle triangle(data.metadata, m_vA, m_vB, m_vC);

	triangle.setNa(m_nA);
	triangle.setNb(m_nB);
	triangle.setNc(m_nC);

	triangle.setUVWa(m_uvwA);
	triangle.setUVWb(m_uvwB);
	triangle.setUVWc(m_uvwC);

	out_primitives.push_back(std::make_unique<PTriangle>(triangle));
}

std::shared_ptr<Geometry> GTriangle::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	auto tTriangle = std::make_shared<GTriangle>(*this);
	transform.transformP(m_vA, &tTriangle->m_vA);
	transform.transformP(m_vB, &tTriangle->m_vB);
	transform.transformP(m_vC, &tTriangle->m_vC);
	transform.transformO(m_nA, &tTriangle->m_nA);
	transform.transformO(m_nB, &tTriangle->m_nB);
	transform.transformO(m_nC, &tTriangle->m_nC);
	tTriangle->m_nA.normalizeLocal();
	tTriangle->m_nB.normalizeLocal();
	tTriangle->m_nC.normalizeLocal();

	return tTriangle;
}

bool GTriangle::isDegenerate() const
{
	return m_vB.sub(m_vA).cross(m_vC.sub(m_vA)).lengthSquared() == 0.0_r;
}

}// end namespace ph
