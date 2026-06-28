#pragma once

#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/DataIO/FileSystem/ResourceIdentifier.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class GIndexedTriangleMesh : public Geometry
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedGeometry& out_geometry) const override;

private:
	ResourceIdentifier m_meshFile;

public:
	PH_DEFINE_SDL_CLASS(GIndexedTriangleMesh, clazz)
	{
		clazz.typeName("indexed-triangle-mesh");
		clazz.docName("Indexed Triangle Mesh");
		clazz.description(
			"A cluster of triangles forming a singe shape in 3-D space. Unlike basic `Triangle Mesh`, "
			"this variant exploit redundancies in mesh vertices resulting in less memory usage in "
			"most cases.");
		clazz.baseOn<Geometry>();

		TSdlResourceIdentifier<OwnerType> meshFile("mesh", &OwnerType::m_meshFile);
		meshFile.description("File that stores the triangle mesh.");
		meshFile.required();
		clazz.addField(meshFile);
	}
};

}// end namespace ph
