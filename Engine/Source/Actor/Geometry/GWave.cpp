#include "Actor/Geometry/GWave.h"
#include "Core/Intersectable/PTriangle.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <iostream>
#include <cmath>

namespace ph
{

GWave::GWave(const real xLen, const real yLen, const real zLen) :
	m_xLen(xLen), m_yLen(yLen), m_zLen(zLen)
{}

GWave::~GWave() = default;

void GWave::genPrimitive(const PrimitiveBuildingMaterial& data,
                         std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_xLen, m_yLen, m_xLen))
	{
		return;
	}

	const int32 numXdivs = 150;
	const int32 numZdivs = 150;

	std::vector<Vector3R> positions;
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

	std::vector<Vector3R> smoothNormals(numXvertices * numZvertices);
	const real dx = m_xLen / static_cast<real>(numXdivs);
	const real dz = m_zLen / static_cast<real>(numZdivs);
	for(int32 iz = 0; iz < numZvertices; iz++)
	{
		for(int32 ix = 0; ix < numXvertices; ix++)
		{
			Vector3R normal(0, 0, 0);
			Vector3R edges[6];

			const Vector3R center(positions[iz * numXvertices + ix]);

			if(ix + 1 < numXvertices)
				edges[0] = Vector3R(dx, positions[iz * numXvertices + (ix + 1)].y - center.y, 0);
			if(ix + 1 < numXvertices && iz + 1 < numZvertices)
				edges[1] = Vector3R(dx, positions[(iz + 1) * numXvertices + (ix + 1)].y - center.y, -dz);
			if(iz + 1 < numZvertices)
				edges[2] = Vector3R(0, positions[(iz + 1) * numXvertices + ix].y - center.y, -dz);
			if(ix - 1 >= 0)
				edges[3] = Vector3R(-dx, positions[iz * numXvertices + (ix - 1)].y - center.y, 0);
			if(ix - 1 >= 0 && iz - 1 >= 0)
				edges[4] = Vector3R(-dx, positions[(iz - 1) * numXvertices + (ix - 1)].y - center.y, dz);
			if(iz - 1 >= 0)
				edges[5] = Vector3R(0, positions[(iz - 1) * numXvertices + ix].y - center.y, dz);

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
			const Vector3R vA(positions[iz * numXvertices + ix]);
			const Vector3R vB(positions[iz * numXvertices + ix + 1]);
			const Vector3R vC(positions[(iz + 1) * numXvertices + ix + 1]);
			const Vector3R vD(positions[(iz + 1) * numXvertices + ix]);

			const Vector3R nA(smoothNormals[iz * numXvertices + ix]);
			const Vector3R nB(smoothNormals[iz * numXvertices + ix + 1]);
			const Vector3R nC(smoothNormals[(iz + 1) * numXvertices + ix + 1]);
			const Vector3R nD(smoothNormals[(iz + 1) * numXvertices + ix]);

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
		const Vector3R vA(-m_xLen / 2.0f, -m_yLen, -m_zLen / 2.0f);// quadrant II
		const Vector3R vB(-m_xLen / 2.0f, -m_yLen, m_zLen / 2.0f);// quadrant III
		const Vector3R vC(m_xLen / 2.0f, -m_yLen, m_zLen / 2.0f);// quadrant IV
		const Vector3R vD(m_xLen / 2.0f, -m_yLen, -m_zLen / 2.0f);// quadrant I
		PTriangle tri1(data.metadata, vA, vD, vB);
		PTriangle tri2(data.metadata, vC, vB, vD);

		// 2 triangles for a rectangle (both CCW)
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}
	
	const real meshSizeX = m_xLen / static_cast<real>(numXdivs);
	const real meshSizeZ = m_zLen / static_cast<real>(numZdivs);
	const real minX = -m_xLen / 2.0_r;
	const real maxZ = m_zLen / 2.0_r;

	// wave front side
	for(int32 ix = 0; ix < numXdivs; ix++)
	{
		const Vector3R vA(positions[ix]);
		const Vector3R vB(minX + static_cast<real>(ix) * meshSizeX, -m_yLen, maxZ);
		const Vector3R vC(minX + static_cast<real>(ix + 1) * meshSizeX, -m_yLen, maxZ);
		const Vector3R vD(positions[ix + 1]);
		PTriangle tri1(data.metadata, vA, vB, vC);
		PTriangle tri2(data.metadata, vA, vC, vD);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}

	// wave back side
	for(int32 ix = 0; ix < numXdivs; ix++)
	{
		const Vector3R vA(positions[numZdivs * numXvertices + ix]);
		const Vector3R vB(minX + static_cast<real>(ix) * meshSizeX, -m_yLen, -maxZ);
		const Vector3R vC(minX + static_cast<real>(ix + 1) * meshSizeX, -m_yLen, -maxZ);
		const Vector3R vD(positions[numZdivs * numXvertices + ix + 1]);
		PTriangle tri1(data.metadata, vA, vC, vB);
		PTriangle tri2(data.metadata, vA, vD, vC);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}

	// wave right side
	for(int32 iz = 0; iz < numZdivs; iz++)
	{
		const Vector3R vA(positions[iz * numXvertices + numXdivs]);
		const Vector3R vB(m_xLen / 2.0_r, -m_yLen, maxZ - static_cast<real>(iz) * meshSizeZ);
		const Vector3R vC(m_xLen / 2.0_r, -m_yLen, maxZ - static_cast<real>(iz + 1) * meshSizeZ);
		const Vector3R vD(positions[(iz + 1) * numXvertices + numXdivs]);
		PTriangle tri1(data.metadata, vA, vB, vC);
		PTriangle tri2(data.metadata, vA, vC, vD);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}

	// wave left side
	for(int32 iz = 0; iz < numZdivs; iz++)
	{
		const Vector3R vA(positions[iz * numXvertices + 0]);
		const Vector3R vB(-m_xLen / 2.0f, -m_yLen, maxZ - static_cast<real>(iz) * meshSizeZ);
		const Vector3R vC(-m_xLen / 2.0f, -m_yLen, maxZ - static_cast<real>(iz + 1) * meshSizeZ);
		const Vector3R vD(positions[(iz + 1) * numXvertices + 0]);
		PTriangle tri1(data.metadata, vA, vC, vB);
		PTriangle tri2(data.metadata, vA, vD, vC);

		// both CCW
		out_primitives.push_back(std::make_unique<PTriangle>(tri1));
		out_primitives.push_back(std::make_unique<PTriangle>(tri2));
	}
}

void GWave::genTessellatedRectangleXZ(const real xLen, const real zLen, const int32 numXdivs, const int32 numZdivs, std::vector<Vector3R>& positions)
{
	if(xLen <= 0.0f || zLen <= 0.0f || numXdivs <= 0 || numZdivs <= 0)
	{
		std::cerr << "warning: at GWave::genTessellateRectangleXZ(), bad input dimensions" << std::endl;
		return;
	}

	const real meshSizeX = xLen / static_cast<real>(numXdivs);
	const real meshSizeZ = zLen / static_cast<real>(numZdivs);
	const real minX = -xLen / 2.0_r;
	const real maxZ =  zLen / 2.0_r;

	for(int32 iz = 0; iz <= numZdivs; iz++)
	{
		for(int32 ix = 0; ix <= numXdivs; ix++)
		{
			const real x = minX + static_cast<real>(ix) * meshSizeX;
			const real y = 0.0_r;
			const real z = maxZ - static_cast<real>(iz) * meshSizeZ;
			positions.push_back(Vector3R(x, y, z));
		}
	}
}

bool GWave::checkData(const PrimitiveBuildingMaterial& data, const real xLen, const real yLen, const real zLen)
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GWave::checkData(), no PrimitiveMetadata" << std::endl;
		return false;
	}

	if(xLen <= 0.0_r || yLen <= 0.0_r || zLen <= 0.0_r)
	{
		std::cerr << "warning: at GWave::checkData(), GWave's dimension is zero or negative" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph