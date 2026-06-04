#pragma once

#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/DataIO/FileSystem/ResourceIdentifier.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Core/Intersection/data_structure_fwd.h"

#include <string_view>
#include <utility>

namespace ph
{

class PlyFile;

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

	const ResourceIdentifier& getPlyFile() const;
	void setPlyFile(Path plyFile);

protected:
	/*!
	@param plyFile The .ply file to load from.
	*/
	IndexedTriangleBuffer loadTriangleBuffer(
		PlyFile& file,
		std::string_view vertexElementName,
		std::string_view positionXPropertyName,
		std::string_view positionYPropertyName,
		std::string_view positionZPropertyName,
		std::string_view normalXPropertyName,
		std::string_view normalYPropertyName,
		std::string_view normalZPropertyName,
		std::string_view faceElementName, 
		std::string_view vertexIndicesPropertyName) const;

	IndexedTriangleBuffer loadStandardTriangleBuffer() const;

private:
	ResourceIdentifier m_plyFile;

public:
	PH_DEFINE_SDL_CLASS(GPlyPolygonMesh, clazz)
	{
		clazz.typeName("ply");
		clazz.docName("PLY Polygon Mesh");
		clazz.description(
			"Polygon mesh stored as a .ply file. This geometry assumes standard data "
			"layout, with \"vertex\" element storing position propreties (x, y, z) and "
			"normal properties (nx, ny, nz); \"face\" element storing a "
			"\"vertex_indices\" property that points into the vertex element to "
			"form polygon faces.");
		clazz.baseOn<Geometry>();

		TSdlResourceIdentifier<OwnerType> plyFile("ply-file", &OwnerType::m_plyFile);
		plyFile.description(
			"The .ply file that stores the polygon mesh.");
		plyFile.required();
		clazz.addField(plyFile);
	}
};

inline const ResourceIdentifier& GPlyPolygonMesh::getPlyFile() const
{
	return m_plyFile;
}

inline void GPlyPolygonMesh::setPlyFile(Path plyFile)
{
	m_plyFile.setPath(std::move(plyFile));
}

}// end namespace ph
