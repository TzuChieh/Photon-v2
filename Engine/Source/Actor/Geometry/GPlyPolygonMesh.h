#pragma once

#include "Actor/Geometry/Geometry.h"
#include "DataIO/SDL/sdl_interface.h"
#include "DataIO/FileSystem/Path.h"
#include "Core/Intersectable/data_structure_fwd.h"

namespace ph
{

/*! @brief Mesh stored as a .ply file.
*/
class GPlyPolygonMesh : public Geometry
{
public:
	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx,
		const GeometryCookConfig& config) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

private:
	Path m_filePath;

	IndexedTriangleBuffer loadTriangleBuffer() const;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GPlyPolygonMesh>)
	{
		ClassType clazz("ply");
		clazz.docName("PLY Polygon Mesh");
		clazz.description("Polygon mesh stored as a .ply file.");
		clazz.baseOn<Geometry>();

		TSdlPath<OwnerType> filePath("file-path", &OwnerType::m_filePath);
		filePath.description(
			"Path to the .ply file that stores the polygon mesh.");
		filePath.required();
		clazz.addField(filePath);

		return clazz;
	}
};

}// end namespace ph
