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
		const CookingContext& ctx,
		CookedGeometry& out_geometry) const override;

	void storeCookedWithBakedTransform(
		const CookingContext& ctx,
		const StaticAffineTransform& transform,
		CookedGeometry& out_geometry) const override;

protected:
	IndexedTriangleBuffer loadTriangleBuffer(
		PlyFile& file,
		const StaticAffineTransform* bakedTransform = nullptr) const;

private:
	static IndexedTriangleBuffer loadDirectlyExpandedBlenderTriangleBuffer(
		PlyFile& file,
		const StaticAffineTransform* bakedTransform);

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
		std::vector<float32> triCustom0;
		std::vector<float32> triCustom1;
		std::vector<float32> triCustom2;
		std::vector<float32> triCustom3;

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

			TSdlFloat32Array<OwnerType> rawVertLoopNormals("raw-vert-loop-normals", &OwnerType::rawVertLoopNormals);
			rawVertLoopNormals.options(EFieldOption::PreferNativeAccess);
			func.addParam(rawVertLoopNormals);

			TSdlFloat32Array<OwnerType> rawVertLoopUVs("raw-vert-loop-uvs", &OwnerType::rawVertLoopUVs);
			rawVertLoopUVs.options(EFieldOption::PreferNativeAccess);
			func.addParam(rawVertLoopUVs);

			TSdlUInt32Array<OwnerType> vertPositionIndices("vert-position-indices", &OwnerType::vertPositionIndices);
			vertPositionIndices.options(EFieldOption::PreferNativeAccess);
			func.addParam(vertPositionIndices);

			TSdlUInt32Array<OwnerType> vertLoopIndices("vert-loop-indices", &OwnerType::vertLoopIndices);
			vertLoopIndices.options(EFieldOption::PreferNativeAccess);
			func.addParam(vertLoopIndices);

			TSdlUInt32Array<OwnerType> triMatIds("tri-mat-ids", &OwnerType::triMatIds);
			triMatIds.options(EFieldOption::PreferNativeAccess);
			func.addParam(triMatIds);

			TSdlFloat32Array<OwnerType> triCustom0("tri-custom-0", &OwnerType::triCustom0);
			triCustom0.description("Optional custom scalar 0 for each triangle.");
			triCustom0.options(EFieldOption::PreferNativeAccess);
			func.addParam(triCustom0);

			TSdlFloat32Array<OwnerType> triCustom1("tri-custom-1", &OwnerType::triCustom1);
			triCustom1.description("Optional custom scalar 1 for each triangle.");
			triCustom1.options(EFieldOption::PreferNativeAccess);
			func.addParam(triCustom1);

			TSdlFloat32Array<OwnerType> triCustom2("tri-custom-2", &OwnerType::triCustom2);
			triCustom2.description("Optional custom scalar 2 for each triangle.");
			triCustom2.options(EFieldOption::PreferNativeAccess);
			func.addParam(triCustom2);

			TSdlFloat32Array<OwnerType> triCustom3("tri-custom-3", &OwnerType::triCustom3);
			triCustom3.description("Optional custom scalar 3 for each triangle.");
			triCustom3.options(EFieldOption::PreferNativeAccess);
			func.addParam(triCustom3);
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
