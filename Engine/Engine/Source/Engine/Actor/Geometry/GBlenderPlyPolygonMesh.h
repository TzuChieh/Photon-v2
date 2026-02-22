#pragma once

#include "Engine/Actor/Geometry/GPlyPolygonMesh.h"
#include "Engine/DataIO/FileSystem/Path.h"
#include "Engine/SDL/sdl_interface.h"

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

		void operator () () const
		{

		}

		PH_DEFINE_SDL_STATIC_METHOD(SdlWritePly, func)
		{
			func.name("write-ply");
			func.description("Writes the polygon mesh to a .ply file.");

			TSdlPath<OwnerType> path("path", &OwnerType::path);
			path.description("Path to write the .ply file.");
			path.required();
			func.addParam(path);
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
