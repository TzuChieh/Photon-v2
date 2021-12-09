#pragma once

#include "Actor/Geometry/Geometry.h"
#include "DataIO/SDL/sdl_interface.h"
#include "DataIO/FileSystem/Path.h"

namespace ph
{

class GIndexedTriangleMesh : public Geometry
{
public:
	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

private:
	Path m_filePath;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GIndexedTriangleMesh>)
	{
		ClassType clazz("indexed-triangle-mesh");
		clazz.docName("Indexed Triangle Mesh");
		clazz.description(
			"A cluster of triangles forming a singe shape in 3-D space. Unlike basic `Triangle Mesh`, "
			"this variant exploit redundancies in mesh vertices resulting in less memory usage in "
			"most cases.");
		clazz.baseOn<Geometry>();

		TSdlPath<OwnerType> filePath("file-path", &OwnerType::m_filePath);
		filePath.description("Path to the file that stores the triangle mesh.");
		filePath.required();
		clazz.addField(filePath);

		return clazz;
	}
};

}// end namespace ph
