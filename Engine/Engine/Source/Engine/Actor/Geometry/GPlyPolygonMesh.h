#pragma once

#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/DataIO/FileSystem/ResourceIdentifier.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Core/Intersection/data_structure_fwd.h"

namespace ph
{

/*! @brief Mesh stored as a .ply file.
*/
class GPlyPolygonMesh : public Geometry
{
public:
	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

	std::shared_ptr<Geometry> genTransformed(
		const StaticAffineTransform& transform) const override;

private:
	ResourceIdentifier m_plyFile;

	IndexedTriangleBuffer loadTriangleBuffer() const;

public:
	PH_DEFINE_SDL_CLASS(GPlyPolygonMesh, clazz)
	{
		clazz.typeName("ply");
		clazz.docName("PLY Polygon Mesh");
		clazz.description("Polygon mesh stored as a .ply file.");
		clazz.baseOn<Geometry>();

		TSdlResourceIdentifier<OwnerType> plyFile("ply-file", &OwnerType::m_plyFile);
		plyFile.description(
			"The .ply file that stores the polygon mesh.");
		plyFile.required();
		clazz.addField(plyFile);
	}
};

}// end namespace ph
