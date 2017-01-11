#include "Actor/Geometry/GCuboid.h"
#include "Core/Primitive/PTriangle.h"
#include "Math/Vector3f.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/TextureMapper/TextureMapper.h"
#include "Actor/AModel.h"

#include <cmath>
#include <iostream>

namespace ph
{

GCuboid::GCuboid(const float32 xLen, const float yLen, const float zLen) :
	m_xLen(xLen), m_yLen(yLen), m_zLen(zLen)
{

}

GCuboid::GCuboid(const GCuboid& other) :
	m_xLen(other.m_xLen), m_yLen(other.m_yLen), m_zLen(other.m_zLen)
{

}

GCuboid::~GCuboid() = default;

void GCuboid::discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const
{
	// TODO: UVW mapping

	const float32 halfXlen = m_xLen * 0.5f;
	const float32 halfYlen = m_yLen * 0.5f;
	const float32 halfZlen = m_zLen * 0.5f;

	// 8 vertices of a cuboid
	const Vector3f vPPP( halfXlen,  halfYlen,  halfZlen);
	const Vector3f vNPP(-halfXlen,  halfYlen,  halfZlen);
	const Vector3f vNNP(-halfXlen, -halfYlen,  halfZlen);
	const Vector3f vPNP( halfXlen, -halfYlen,  halfZlen);
	const Vector3f vPPN( halfXlen,  halfYlen, -halfZlen);
	const Vector3f vNPN(-halfXlen,  halfYlen, -halfZlen);
	const Vector3f vNNN(-halfXlen, -halfYlen, -halfZlen);
	const Vector3f vPNN( halfXlen, -halfYlen, -halfZlen);

	// 12 triangles (all CCW)

	// xy-plane
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPPP, vNPP, vNNP));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPPP, vNNP, vPNP));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vNPN, vPPN, vPNN));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vNPN, vPNN, vNNN));

	// yz-plane
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPPN, vPPP, vPNP));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPPN, vPNP, vPNN));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vNPP, vNPN, vNNN));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vNPP, vNNN, vNNP));

	// xz-plane
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPPN, vNPN, vNPP));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPPN, vNPP, vPPP));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPNP, vNNP, vNNN));
	out_primitives->push_back(std::make_unique<PTriangle>(&metadata, vPNP, vNNN, vPNN));
}

GCuboid& GCuboid::operator = (const GCuboid& rhs)
{
	m_xLen = rhs.m_xLen;
	m_yLen = rhs.m_yLen;
	m_zLen = rhs.m_zLen;

	return *this;
}

}// end namespace ph