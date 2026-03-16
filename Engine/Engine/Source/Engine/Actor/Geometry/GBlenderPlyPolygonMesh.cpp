#include "Engine/Actor/Geometry/GBlenderPlyPolygonMesh.h"
#include "Engine/DataIO/PlyFile.h"
#include "Engine/DataIO/Stream/BinaryFileOutputStream.h"

#include <Common/assertion.h>

#include <stdio.h>
#include <array>
#include <filesystem>
#include <format>

namespace ph
{

void GBlenderPlyPolygonMesh::SdlWritePly::operator () () const
{
	PH_ASSERT_EQ(rawVertPositions.size() % 3, 0);
	PH_ASSERT_EQ(rawVertLoopNormals.size() % 3, 0);
	PH_ASSERT_EQ(rawVertLoopUVs.size() % 2, 0);
	PH_ASSERT_EQ(vertPositionIndices.size() % 3, 0);
	PH_ASSERT_EQ(vertLoopIndices.size() % 3, 0);
	PH_ASSERT_EQ(triMatIds.size() % 3, 0);

	const auto numPosVerts = rawVertPositions.size() / 3;
	const auto numLoopVerts = rawVertLoopNormals.size() / 3;
	const auto numTris = vertLoopIndices.size() / 3;

#if PH_DEBUG
	if((numLoopVerts != rawVertLoopUVs.size() / 2) || 
	   (triMatIds.size() != vertPositionIndices.size() || triMatIds.size() != vertLoopIndices.size()))
	{
		PH_DEFAULT_LOG(Warning,
			"Inconsistent Blender PLY polygon data sizes: "
			"raw-vert-positions={}, raw-vert-normals={}, raw-vert-uvs={}, vert-position-indices={}, vert-loop-indices={}, tri-mat-ids={}",
			rawVertPositions.size(), rawVertLoopNormals.size(), rawVertLoopUVs.size(), vertPositionIndices.size(), vertLoopIndices.size(), triMatIds.size());
	}
#endif

	BinaryFileOutputStream writeStream(path);

	// Write header
	writeStream.writeData<char>(std::format(
		"ply\n"
		"format binary_little_endian 1.0\n"
		"element raw_vert_positions {}\n"
		"property float x\n"
		"property float y\n"
		"property float z\n"
		"element raw_vert_loop_normals {}\n"
		"property float nx\n"
		"property float ny\n"
		"property float nz\n"
		"element raw_vert_loop_uvs {}\n"
		"property float u\n"
		"property float v\n"
		"element position_indices {}\n"
		"property uint pi\n"
		"element loop_indices {}\n"
		"property uint li\n"
		"element mat_ids {}\n"
		"property uint mi\n",
		numPosVerts,
		numLoopVerts,
		numLoopVerts,
		numTris,
		numTris,
		numTris));

	// Write actual data
	writeStream.writeData<float32>(rawVertPositions);
	writeStream.writeData<float32>(rawVertLoopNormals);
	writeStream.writeData<float32>(rawVertLoopUVs);
	writeStream.writeData<uint32>(vertPositionIndices);
	writeStream.writeData<uint32>(vertLoopIndices);
	writeStream.writeData<uint32>(triMatIds);
}

void GBlenderPlyPolygonMesh::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
{
	// TODO
}

}// end namespace ph
