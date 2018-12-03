#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "FileIO/SDL/InputPacket.h"
#include "Actor/Geometry/GeometrySoup.h"
#include "Common/Logger.h"
#include "Core/Intersectable/TriangleKdtree/TriangleKdtree.h"
#include "Utility/Timer.h"

#include <iostream>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Triangle Mesh"));
}

GTriangleMesh::GTriangleMesh() : 
	Geometry(), 
	m_gTriangles(),
	m_useTriangleKdtree(false)
{}

GTriangleMesh::GTriangleMesh(const std::vector<Vector3R>& positions,
                             const std::vector<Vector3R>& texCoords,
                             const std::vector<Vector3R>& normals) : 
	GTriangleMesh()
{
	if(!(positions.size() == texCoords.size() && texCoords.size() == normals.size()) ||
	    (positions.empty() || texCoords.empty() || normals.empty()) ||
	    (positions.size() % 3 != 0 || texCoords.size() % 3 != 0 || normals.size() % 3 != 0))
	{
		std::cerr << "warning: at GTriangleMesh::GTriangleMesh(), " 
		          << "bad input detected" << std::endl;
		return;
	}

	for(std::size_t i = 0; i < positions.size(); i += 3)
	{
		GTriangle triangle(positions[i + 0], positions[i + 1], positions[i + 2]);
		if(triangle.isDegenerate())
		{
			continue;
		}

		triangle.setUVWa(texCoords[i + 0]);
		triangle.setUVWb(texCoords[i + 1]);
		triangle.setUVWc(texCoords[i + 2]);
		triangle.setNa(normals[i + 0].lengthSquared() > 0 ? normals[i + 0].normalize() : Vector3R(0, 1, 0));
		triangle.setNb(normals[i + 1].lengthSquared() > 0 ? normals[i + 1].normalize() : Vector3R(0, 1, 0));
		triangle.setNc(normals[i + 2].lengthSquared() > 0 ? normals[i + 2].normalize() : Vector3R(0, 1, 0));
		addTriangle(triangle);
	}
}

GTriangleMesh::~GTriangleMesh() = default;

void GTriangleMesh::genPrimitive(const PrimitiveBuildingMaterial& data,
                                 std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!m_useTriangleKdtree)
	{
		for(const auto& gTriangle : m_gTriangles)
		{
			gTriangle.genPrimitive(data, out_primitives);
		}
	}
	else
	{
		logger.log("start building triangle kD-tree...");

		Timer timer;
		timer.start();

		Triangles triangles;
		for(const auto& gTriangle : m_gTriangles)
		{
			// FIXME: leak
			Triangle* triangle = new Triangle();
			triangle->setTvertices(
				gTriangle.getVa().x, 
				gTriangle.getVa().y,
				gTriangle.getVa().z,
				gTriangle.getVb().x,
				gTriangle.getVb().y,
				gTriangle.getVb().z, 
				gTriangle.getVc().x,
				gTriangle.getVc().y,
				gTriangle.getVc().z);

			triangles.tris.push_back(triangle);
		}

		auto kdNode = std::make_unique<KDNode>(data.metadata);
		kdNode->build_KD_tree(triangles);
		out_primitives.push_back(std::move(kdNode));

		timer.finish();

		logger.log("kD-tree build time: " + std::to_string(timer.getDeltaMs()) + " ms");
	}
}

void GTriangleMesh::addTriangle(const GTriangle& gTriangle)
{
	m_gTriangles.push_back(gTriangle);
}

void GTriangleMesh::useTriangleKdtree(const bool value)
{
	m_useTriangleKdtree = value;
}

std::shared_ptr<Geometry> GTriangleMesh::genTransformApplied(const StaticAffineTransform& transform) const
{
	auto geometrySoup = std::make_shared<GeometrySoup>();
	for(const auto& gTriangle : m_gTriangles)
	{
		geometrySoup->addGeometry(std::make_shared<GTriangle>(gTriangle));
	}

	return geometrySoup->genTransformApplied(transform);
}

// command interface

SdlTypeInfo GTriangleMesh::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "triangle-mesh");
}

void GTriangleMesh::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<GTriangleMesh>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<GTriangleMesh> GTriangleMesh::ciLoad(const InputPacket& packet)
{
	const std::vector<Vector3R> positions = packet.getVector3Array("positions");
	const std::vector<Vector3R> texCoords = packet.getVector3Array("texture-coordinates");
	const std::vector<Vector3R> normals   = packet.getVector3Array("normals");

	auto triangleMesh = std::make_unique<GTriangleMesh>(positions, texCoords, normals);

	if(packet.getString("use-triangle-kdtree") == "true")
	{
		triangleMesh->useTriangleKdtree(true);
	}

	return triangleMesh;
}

}// end namespace ph