#include "Actor/Geometry/GWave.h"
#include "Core/Primitive/PTriangle.h"
#include "Math/Vector3f.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <iostream>
#include <cmath>

namespace ph
{

GWave::GWave(const float32 xLen, const float32 yLen, const float32 zLen) :
	m_xLen(xLen), m_yLen(yLen), m_zLen(zLen)
{

}

GWave::~GWave() = default;

void GWave::discretize(const PrimitiveBuildingMaterial& data,
                       std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_xLen, m_yLen, m_xLen))
	{
		return;
	}

	const int32 numXdivs = 150;
	const int32 numZdivs = 150;

	std::vector<Vector3f> positions;
	genTessellatedRectangleXZ(m_xLen, m_zLen, numXdivs, numZdivs, positions);

	for(auto& pos : positions)
	{
		pos.y = 0.0f;
		pos.y += std::sin(pos.x * pos.x + pos.z * pos.z) * 0.1f;
		pos.y -= std::sin((pos.x / 2.0f + pos.z * 0.1f) * 7.0f) * 0.4f;
		pos.y += std::cos((pos.z / 1.5f + pos.x * 0.3f) * 5.0f) * 0.46f;
		pos.y -= std::cos((pos.z / 1.5f + pos.x * 0.3f) * 12.0f) * 0.07f;
		pos.y += std::sin((pos.x + pos.z * 0.5f) * 4.0f) * 0.3f;
		pos.y -= std::sin((pos.x * 0.2f + pos.z * 0.8f) * 10.0f) * 0.22f;
		pos.y += std::cos((pos.x * -0.5f + pos.z * 0.5f) * 7.0f) * 0.32f;
		pos.y /= 3.5f;
	}

	const int32 numXvertices = numXdivs + 1;
	const int32 numZvertices = numZdivs + 1;

	std::vector<Vector3f> smoothNormals(numXvertices * numZvertices);
	const float32 dx = m_xLen / static_cast<float32>(numXdivs);
	const float32 dz = m_zLen / static_cast<float32>(numZdivs);
	for(int32 iz = 0; iz < numZvertices; iz++)
	{
		for(int32 ix = 0; ix < numXvertices; ix++)
		{
			Vector3f normal(0, 0, 0);
			Vector3f edges[6];

			const Vector3f center(positions[iz * numXvertices + ix]);

			if(ix + 1 < numXvertices)
				edges[0] = Vector3f(dx, positions[iz * numXvertices + (ix + 1)].y - center.y, 0);
			if(ix + 1 < numXvertices && iz + 1 < numZvertices)
				edges[1] = Vector3f(dx, positions[(iz + 1) * numXvertices + (ix + 1)].y - center.y, -dz);
			if(iz + 1 < numZvertices)
				edges[2] = Vector3f(0, positions[(iz + 1) * numXvertices + ix].y - center.y, -dz);
			if(ix - 1 >= 0)
				edges[3] = Vector3f(-dx, positions[iz * numXvertices + (ix - 1)].y - center.y, 0);
			if(ix - 1 >= 0 && iz - 1 >= 0)
				edges[4] = Vector3f(-dx, positions[(iz - 1) * numXvertices + (ix - 1)].y - center.y, dz);
			if(iz - 1 >= 0)
				edges[5] = Vector3f(0, positions[(iz - 1) * numXvertices + ix].y - center.y, dz);

			for(int32 ei = 0; ei < 5; ei++)
			{
				normal.addLocal(edges[ei].cross(edges[ei + 1]));
			}
			normal.addLocal(edges[5].cross(edges[0]));

			smoothNormals[iz * numXvertices + ix] = normal.normalizeLocal();
		}
	}
	
	// wave top surface
	for(int32 iz = 0; iz < numZdivs; iz++)
	{
		for(int32 ix = 0; ix < numXdivs; ix++)
		{
			const Vector3f vA(positions[iz * numXvertices + ix]);
			const Vector3f vB(positions[iz * numXvertices + ix + 1]);
			const Vector3f vC(positions[(iz + 1) * numXvertices + ix + 1]);
			const Vector3f vD(positions[(iz + 1) * numXvertices + ix]);

			const Vector3f nA(smoothNormals[iz * numXvertices + ix]);
			const Vector3f nB(smoothNormals[iz * numXvertices + ix + 1]);
			const Vector3f nC(smoothNormals[(iz + 1) * numXvertices + ix + 1]);
			const Vector3f nD(smoothNormals[(iz + 1) * numXvertices + ix]);

			PTriangle tri1(data.metadata, vA, vB, vC);
			tri1.setNa(nA);
			tri1.setNb(nB);
			tri1.setNc(nC);

			PTriangle tri2(data.metadata, vA, vC, vD);
			tri2.setNa(nA);
			tri2.setNb(nC);
			tri2.setNc(nD);

			// 2 triangles for a mesh (both CCW)
			out_primitives.push_back(std::make_unique<PTriangle>(tri1));
			out_primitives.push_back(std::make_unique<PTriangle>(tri2));
		}
	}

	// wave bottom rectangle
	{
		const Vector3f vA(-m_xLen / 2.0f, -m_yLen, -m_zLen / 2.0f);// quadrant II
		const Vector3f vB(-m_xLen / 2.0f, -m_yLen, m_zLen / 2.0f);// quadrant III
		const Vector3f vC(m_xLen / 2.0f, -m_yLen, m_zLen / 2.0f);// quadrant IV
		const Vector3f vD(m_xLen / 2.0f, -m_yLen, -m_zLen / 2.0f);// quadrant I
		PTriangle tri1(data.metadata, vA, vD, vB);
		PTriangle tri2(data.metadata, vC, vB, vD);

		// 2 triangles for a rectangle (both CCW)
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}
	
	const float32 meshSizeX = m_xLen / static_cast<float32>(numXdivs);
	const float32 meshSizeZ = m_zLen / static_cast<float32>(numZdivs);
	const float32 minX = -m_xLen / 2.0f;
	const float32 maxZ = m_zLen / 2.0f;

	// wave front side
	for(int32 ix = 0; ix < numXdivs; ix++)
	{
		const Vector3f vA(positions[ix]);
		const Vector3f vB(minX + static_cast<float32>(ix) * meshSizeX, -m_yLen, maxZ);
		const Vector3f vC(minX + static_cast<float32>(ix + 1) * meshSizeX, -m_yLen, maxZ);
		const Vector3f vD(positions[ix + 1]);
		PTriangle tri1(data.metadata, vA, vB, vC);
		PTriangle tri2(data.metadata, vA, vC, vD);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}

	// wave back side
	for(int32 ix = 0; ix < numXdivs; ix++)
	{
		const Vector3f vA(positions[numZdivs * numXvertices + ix]);
		const Vector3f vB(minX + static_cast<float32>(ix) * meshSizeX, -m_yLen, -maxZ);
		const Vector3f vC(minX + static_cast<float32>(ix + 1) * meshSizeX, -m_yLen, -maxZ);
		const Vector3f vD(positions[numZdivs * numXvertices + ix + 1]);
		PTriangle tri1(data.metadata, vA, vC, vB);
		PTriangle tri2(data.metadata, vA, vD, vC);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}

	// wave right side
	for(int32 iz = 0; iz < numZdivs; iz++)
	{
		const Vector3f vA(positions[iz * numXvertices + numXdivs]);
		const Vector3f vB(m_xLen / 2.0f, -m_yLen, maxZ - static_cast<float32>(iz) * meshSizeZ);
		const Vector3f vC(m_xLen / 2.0f, -m_yLen, maxZ - static_cast<float32>(iz + 1) * meshSizeZ);
		const Vector3f vD(positions[(iz + 1) * numXvertices + numXdivs]);
		PTriangle tri1(data.metadata, vA, vB, vC);
		PTriangle tri2(data.metadata, vA, vC, vD);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}

	// wave left side
	for(int32 iz = 0; iz < numZdivs; iz++)
	{
		const Vector3f vA(positions[iz * numXvertices + 0]);
		const Vector3f vB(-m_xLen / 2.0f, -m_yLen, maxZ - static_cast<float32>(iz) * meshSizeZ);
		const Vector3f vC(-m_xLen / 2.0f, -m_yLen, maxZ - static_cast<float32>(iz + 1) * meshSizeZ);
		const Vector3f vD(positions[(iz + 1) * numXvertices + 0]);
		PTriangle tri1(data.metadata, vA, vC, vB);
		PTriangle tri2(data.metadata, vA, vD, vC);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}
}

void GWave::genTessellatedRectangleXZ(const float32 xLen, const float32 zLen, const int32 numXdivs, const int32 numZdivs, std::vector<Vector3f>& positions)
{
	if(xLen <= 0.0f || zLen <= 0.0f || numXdivs <= 0 || numZdivs <= 0)
	{
		std::cerr << "warning: at GWave::genTessellateRectangleXZ(), bad input dimensions" << std::endl;
		return;
	}

	const float32 meshSizeX = xLen / static_cast<float32>(numXdivs);
	const float32 meshSizeZ = zLen / static_cast<float32>(numZdivs);
	const float32 minX = -xLen / 2.0f;
	const float32 maxZ =  zLen / 2.0f;

	for(int32 iz = 0; iz <= numZdivs; iz++)
	{
		for(int32 ix = 0; ix <= numXdivs; ix++)
		{
			const float32 x = minX + static_cast<float32>(ix) * meshSizeX;
			const float32 y = 0.0f;
			const float32 z = maxZ - static_cast<float32>(iz) * meshSizeZ;
			positions.push_back(Vector3f(x, y, z));
		}
	}
}

bool GWave::checkData(const PrimitiveBuildingMaterial& data, const float32 xLen, const float32 yLen, const float32 zLen)
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GWave::checkData(), no PrimitiveMetadata" << std::endl;
		return false;
	}

	if(xLen <= 0.0f || yLen <= 0.0f || zLen <= 0.0f)
	{
		std::cerr << "warning: at GWave::checkData(), GWave's dimension is zero or negative" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph