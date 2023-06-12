#pragma once

#include "Actor/Geometry/Geometry.h"
#include "DataIO/FileSystem/ResourceIdentifier.h"
#include "SDL/sdl_interface.h"
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
	ResourceIdentifier m_plyFile;

	IndexedTriangleBuffer loadTriangleBuffer() const;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<GPlyPolygonMesh>)
	{
		ClassType clazz("ply");
		clazz.docName("PLY Polygon Mesh");
		clazz.description("Polygon mesh stored as a .ply file.");
		clazz.baseOn<Geometry>();

		TSdlResourceIdentifier<OwnerType> plyFile("ply-file", &OwnerType::m_plyFile);
		plyFile.description(
			"The .ply file that stores the polygon mesh.");
		plyFile.required();
		clazz.addField(plyFile);

		return clazz;
	}
};

}// end namespace ph
