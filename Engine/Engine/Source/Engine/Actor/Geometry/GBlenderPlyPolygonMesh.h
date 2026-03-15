#pragma once

#include "Engine/Actor/Geometry/GPlyPolygonMesh.h"
#include "Engine/DataIO/FileSystem/Path.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

/*! @brief Polygon mesh exported from Blender and stored as a .ply file.
*/
class GBlenderPlyPolygonMesh : public GPlyPolygonMesh
{
public:
	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

public:
	struct SdlWritePly
	{
		Path path;
		std::vector<float32> rawVertPositions;
		std::vector<float32> rawVertLoopNormals;
		std::vector<float32> rawVertLoopUVs;
		std::vector<uint32> vertPositionIndices;
		std::vector<uint32> vertLoopIndices;
		std::vector<uint32> triMatIds;

		void operator () () const;

		PH_DEFINE_SDL_STATIC_METHOD(SdlWritePly, func)
		{
			func.name("write-ply");
			func.description("Writes the polygon mesh to a .ply file.");

			TSdlPath<OwnerType> path("path", &OwnerType::path);
			path.description("Path to write the .ply file.");
			path.required();
			func.addParam(path);

			TSdlFloat32Array<OwnerType> rawVertPositions("raw-vert-positions", &OwnerType::rawVertPositions);
			rawVertPositions.options(EFieldOption::PreferNativeAccess);
			func.addParam(rawVertPositions);

			TSdlFloat32Array<OwnerType> rawVertLoopNormals("raw-vert-normals", &OwnerType::rawVertLoopNormals);
			rawVertLoopNormals.options(EFieldOption::PreferNativeAccess);
			func.addParam(rawVertLoopNormals);

			TSdlFloat32Array<OwnerType> rawVertLoopUVs("raw-vert-uvs", &OwnerType::rawVertLoopUVs);
			rawVertLoopUVs.options(EFieldOption::PreferNativeAccess);
			func.addParam(rawVertLoopUVs);

			TSdlUInt32Array<OwnerType> vertPositionIndices("vert-position-indices", &OwnerType::vertPositionIndices);
			vertPositionIndices.options(EFieldOption::PreferNativeAccess);
			func.addParam(vertPositionIndices);

			TSdlUInt32Array<OwnerType> vertLoopIndices("vert-loop-indices", &OwnerType::vertPositionIndices);
			vertLoopIndices.options(EFieldOption::PreferNativeAccess);
			func.addParam(vertLoopIndices);

			TSdlUInt32Array<OwnerType> triMatIds("tri-mat-ids", &OwnerType::vertPositionIndices);
			triMatIds.options(EFieldOption::PreferNativeAccess);
			func.addParam(triMatIds);
		}
	};

	PH_DEFINE_SDL_CLASS(GBlenderPlyPolygonMesh, clazz, interface=python)
	{
		clazz.typeName("blender-ply");
		clazz.docName("Blender PLY Polygon Mesh");
		clazz.description("Polygon mesh exported from Blender and stored as a .ply file.");
		clazz.baseOn<GPlyPolygonMesh>();

		clazz.addFunction<SdlWritePly>();
	}
};

}// end namespace ph
