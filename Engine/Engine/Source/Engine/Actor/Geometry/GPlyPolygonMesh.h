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
class IndexedAttributeBuffer;
class IndexedAttributeBufferWriter;

/*! @brief Mesh stored as a .ply file.
*/
class GPlyPolygonMesh : public Geometry
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedGeometry& out_geometry) const override;

	void storeCookedWithBakedTransform(
		const CookingContext& ctx,
		const StaticAffineTransform& transform,
		CookedGeometry& out_geometry) const override;

	const ResourceIdentifier& getPlyFile() const;
	void setPlyFile(Path plyFile);

protected:
	/*! @brief Load a triangle buffer from named PLY properties.
	Pass an empty string view (`{}`) for an unused property.
	@param file The .ply file to load from.
	@param tangentSignPropertyName Name of the optional property whose sign
	controls the MikkTSpace bitangent. Values must be finite and nonzero.
	A missing property implies default handedness in the source mesh.
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
		std::string_view tangentXPropertyName,
		std::string_view tangentYPropertyName,
		std::string_view tangentZPropertyName,
		std::string_view tangentSignPropertyName,
		std::string_view faceElementName, 
		std::string_view vertexIndicesPropertyName,
		const StaticAffineTransform* bakedTransform = nullptr) const;

	IndexedTriangleBuffer loadStandardTriangleBuffer(
		const StaticAffineTransform* bakedTransform = nullptr) const;

	static void applyBakedTransform(
		const IndexedAttributeBuffer& srcAttributes,
		IndexedAttributeBufferWriter& dstAttributes,
		const StaticAffineTransform& transform);

	static void storeCookedPolygonMesh(
		const CookingContext& ctx,
		IndexedTriangleBuffer triangleBuffer,
		CookedGeometry& out_geometry);

private:
	ResourceIdentifier m_plyFile;

public:
	PH_DEFINE_SDL_CLASS(GPlyPolygonMesh, clazz)
	{
		clazz.typeName("ply");
		clazz.docName("PLY Polygon Mesh");
		clazz.description(
			"Polygon mesh stored as a .ply file. This geometry assumes standard data "
			"layout, with \"vertex\" element storing position properties (x, y, z) and "
			"optional normal properties (nx, ny, nz) and tangent properties (tx, ty, tz), with "
			"an optional tw providing bitangent handedness; "
			"\"face\" element stores a \"vertex_indices\" property that points into "
			"the vertex element to form polygon faces.");
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
