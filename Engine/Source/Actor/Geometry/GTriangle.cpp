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

GTriangle::GTriangle(const Vector3R& vA, const Vector3R& vB, const Vector3R& vC) : 
	m_vA(vA), m_vB(vB), m_vC(vC)
{
	const Vector3R faceN = vB.sub(vA).cross(vC.sub(vA)).normalizeLocal();
	m_nA = faceN;
	m_nB = faceN;
	m_nC = faceN;

	m_uvwA.set(0.0_r);
	m_uvwB.set(0.0_r);
	m_uvwC.set(0.0_r);
}

GTriangle::GTriangle(const InputPacket& packet) : 
	Geometry(packet)
{
	const std::string notFoundMessage = "at GTriangle::GTriangle(), unexpected result is likely to occur";

	m_vA   = packet.getVector3r("vA",   Vector3R(0), notFoundMessage);
	m_vB   = packet.getVector3r("vB",   Vector3R(0), notFoundMessage);
	m_vC   = packet.getVector3r("vC",   Vector3R(0), notFoundMessage);
	m_nC   = packet.getVector3r("nA",   Vector3R(0), notFoundMessage);
	m_nC   = packet.getVector3r("nB",   Vector3R(0), notFoundMessage);
	m_nC   = packet.getVector3r("nC",   Vector3R(0), notFoundMessage);
	m_uvwA = packet.getVector3r("uvwA", Vector3R(0), notFoundMessage);
	m_uvwB = packet.getVector3r("uvwB", Vector3R(0), notFoundMessage);
	m_uvwC = packet.getVector3r("uvwC", Vector3R(0), notFoundMessage);
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

	Vector3R mappedUVW;

	m_textureMapper->map(m_vA, m_uvwA, &mappedUVW);
	triangle.setUVWa(mappedUVW);

	m_textureMapper->map(m_vB, m_uvwB, &mappedUVW);
	triangle.setUVWb(mappedUVW);

	m_textureMapper->map(m_vC, m_uvwC, &mappedUVW);
	triangle.setUVWc(mappedUVW);

	out_primitives.push_back(std::make_unique<PTriangle>(triangle));
}

}// end namespace ph