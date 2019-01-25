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
#include <fstream>
#include <unistd.h>
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
	if(positions.empty() || positions.size() % 3 != 0)
	{
		logger.log(ELogLevel::WARNING_MED, "position buffer is empty or ill-formed");
		return;
	}

	// now we are sure that position buffer is with valid format

	// FIXME: duplicated checking code

	const bool isTexCoordsGood = texCoords.size() == positions.size();
	if(!isTexCoordsGood)
	{
		if(texCoords.empty())
		{
			logger.log("texture coordinates buffer is empty, using default value");
		}
		else
		{
			logger.log(ELogLevel::WARNING_MED, 
				"texture coordinates buffer is ill-formed, using default value");
		}
	}

	const bool isNormalsGood = normals.size() == positions.size();
	if(!isNormalsGood)
	{
		if(normals.empty())
		{
			logger.log("normal buffer is empty, using face normal");
		}
		else
		{
			logger.log(ELogLevel::WARNING_MED, 
				"normal buffer is ill-formed, using face normal");
		}
	}

	for(std::size_t i = 0; i < positions.size(); i += 3)
	{
		GTriangle triangle(positions[i + 0], positions[i + 1], positions[i + 2]);
		if(triangle.isDegenerate())
		{
			continue;
		}

		// TODO: we should rely on GTriangle's default value instead

		if(isTexCoordsGood)
		{
			triangle.setUVWa(texCoords[i + 0]);
			triangle.setUVWb(texCoords[i + 1]);
			triangle.setUVWc(texCoords[i + 2]);
		}
		else
		{
			triangle.setUVWa(Vector3R(0));
			triangle.setUVWb(Vector3R(0));
			triangle.setUVWc(Vector3R(0));
		}
		
		if(isNormalsGood)
		{
			triangle.setNa(normals[i + 0].lengthSquared() > 0 ? normals[i + 0].normalize() : Vector3R(0, 1, 0));
			triangle.setNb(normals[i + 1].lengthSquared() > 0 ? normals[i + 1].normalize() : Vector3R(0, 1, 0));
			triangle.setNc(normals[i + 2].lengthSquared() > 0 ? normals[i + 2].normalize() : Vector3R(0, 1, 0));
		}
		else
		{
			const Vector3R faceNormal = triangle.calcFaceNormal();
			triangle.setNa(faceNormal);
			triangle.setNb(faceNormal);
			triangle.setNc(faceNormal);
		}
		
		addTriangle(triangle);
	}
}

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
			triangle->setVertices(
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

		// DEBUG
		// export triangle obj file
		{
			std::ofstream triangleObj("./triangles.obj");

			for(std::size_t ti = 0; ti < triangles.tris.size(); ++ti)
			{
				Triangle* triangle = triangles.tris[ti];

				for(std::size_t vi = 0; vi < 3; ++vi)
				{
					const Vector3R vertex = triangle->getVerticies()[vi];

					triangleObj << "v "
						<< std::to_string(vertex.x) << ' '
						<< std::to_string(vertex.y) << ' '
						<< std::to_string(vertex.z) << std::endl;
				}

				triangleObj << "f "
					<< std::to_string(3 * ti + 1) << ' '
					<< std::to_string(3 * ti + 2) << ' '
					<< std::to_string(3 * ti + 3) << std::endl;
			}
		}

		auto KdAccel = std::make_unique<KDAccel>(data.metadata);
		//KDNode kdNode(data.metadata);
		KdAccel.get()->root = std::make_unique<KDNode>(data.metadata);
		KdAccel.get()->build_KD_tree(triangles,data.metadata);
		printf("kdNode->left:%p\n",KdAccel.get()->root.get()->left);
		printf("kdNode->right:%p\n",KdAccel.get()->root.get()->right);
		// DEBUG
		KdAccel.get()->root->exportObj();

		out_primitives.push_back(std::move(KdAccel.get()->root));

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