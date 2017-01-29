#include "Actor/Geometry/GTriangle.h"
#include "Core/Primitive/PTriangle.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/TextureMapper/TextureMapper.h"
#include "Actor/AModel.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <iostream>

namespace ph
{

GTriangle::GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC) : 
	m_vA(vA), m_vB(vB), m_vC(vC)
{
	const Vector3f faceN = vB.sub(vA).cross(vC.sub(vA)).normalizeLocal();
	m_nA = faceN;
	m_nB = faceN;
	m_nC = faceN;

	m_uvwA.set(0.0f);
	m_uvwB.set(0.0f);
	m_uvwC.set(0.0f);
}

GTriangle::GTriangle(const InputPacket& packet) : 
	Geometry(packet)
{
	/*m_vA = packet.getVector3r("vA", Vector3f());
	m_vB = packet.getVector3r("vB", Vector3f());
	m_vC = packet.getVector3r("vC", Vector3f());*/

	std::cerr << "GTriangle::GTriangle(const InputPacket& packet) not implemented!" << std::endl;
}

GTriangle::~GTriangle() = default;

void GTriangle::discretize(const PrimitiveBuildingMaterial& data,
                           std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GTriangle::discretize(), no PrimitiveMetadata" << std::endl;
		return;
	}

	PTriangle triangle(data.metadata, m_vA, m_vB, m_vC);

	triangle.setNa(m_nA);
	triangle.setNb(m_nB);
	triangle.setNc(m_nC);

	Vector3f mappedUVW;

	m_textureMapper->map(m_vA, m_uvwA, &mappedUVW);
	triangle.setUVWa(mappedUVW);

	m_textureMapper->map(m_vB, m_uvwB, &mappedUVW);
	triangle.setUVWb(mappedUVW);

	m_textureMapper->map(m_vC, m_uvwC, &mappedUVW);
	triangle.setUVWc(mappedUVW);

	out_primitives.push_back(std::make_unique<PTriangle>(triangle));
}

}// end namespace ph