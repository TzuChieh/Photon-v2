#include "Actor/Geometry/GCuboid.h"
#include "Core/Primitive/PTriangle.h"
#include "Math/TVector3.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/TextureMapper/TextureMapper.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <cmath>
#include <iostream>

namespace ph
{

GCuboid::GCuboid(const float32 xLen, const float32 yLen, const float32 zLen) :
	m_xLen(xLen), m_yLen(yLen), m_zLen(zLen)
{

}

GCuboid::GCuboid(const GCuboid& other) :
	m_xLen(other.m_xLen), m_yLen(other.m_yLen), m_zLen(other.m_zLen)
{

}

GCuboid::~GCuboid() = default;

void GCuboid::discretize(const PrimitiveBuildingMaterial& data,
                         std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_xLen, m_yLen, m_zLen))
	{
		return;
	}

	// TODO: UVW mapping

	const float32 halfXlen = m_xLen * 0.5f;
	const float32 halfYlen = m_yLen * 0.5f;
	const float32 halfZlen = m_zLen * 0.5f;

	// 8 vertices of a cuboid
	const Vector3R vPPP( halfXlen,  halfYlen,  halfZlen);
	const Vector3R vNPP(-halfXlen,  halfYlen,  halfZlen);
	const Vector3R vNNP(-halfXlen, -halfYlen,  halfZlen);
	const Vector3R vPNP( halfXlen, -halfYlen,  halfZlen);
	const Vector3R vPPN( halfXlen,  halfYlen, -halfZlen);
	const Vector3R vNPN(-halfXlen,  halfYlen, -halfZlen);
	const Vector3R vNNN(-halfXlen, -halfYlen, -halfZlen);
	const Vector3R vPNN( halfXlen, -halfYlen, -halfZlen);

	// 12 triangles (all CCW)

	// xy-plane
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPP, vNPP, vNNP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPP, vNNP, vPNP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPN, vPPN, vPNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPN, vPNN, vNNN));

	// yz-plane
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vPPP, vPNP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vPNP, vPNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPP, vNPN, vNNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPP, vNNN, vNNP));

	// xz-plane
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vNPN, vNPP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vNPP, vPPP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPNP, vNNP, vNNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPNP, vNNN, vPNN));
}

GCuboid& GCuboid::operator = (const GCuboid& rhs)
{
	m_xLen = rhs.m_xLen;
	m_yLen = rhs.m_yLen;
	m_zLen = rhs.m_zLen;

	return *this;
}

bool GCuboid::checkData(const PrimitiveBuildingMaterial& data, const float32 xLen, const float32 yLen, const float32 zLen)
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GCuboid::checkData(), no PrimitiveMetadata" << std::endl;
		return false;
	}

	if(xLen <= 0.0f || yLen <= 0.0f || zLen <= 0.0f)
	{
		std::cerr << "warning: at GCuboid::checkData(), GCuboid's dimension is zero or negative" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph