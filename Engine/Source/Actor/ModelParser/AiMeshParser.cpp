#include "Actor/ModelParser/AiMeshParser.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/GTriangle.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/TextureMapper/DefaultMapper.h"

#include <vector>
#include <iostream>

namespace ph
{

std::shared_ptr<Geometry> AiMeshParser::parse(const aiMesh* const mesh)
{
	std::shared_ptr<GTriangleMesh> triangleMesh = std::make_shared<GTriangleMesh>();

	uint32 numUVWChannels = static_cast<uint32>(mesh->GetNumUVChannels());
	if(numUVWChannels > 1)
	{
		std::cerr << "warning: at AiMeshParser::parse(), mesh has > 1 uvw channels, channel ID's other than 0 will not be parsed" << std::endl;
	}

	for(uint32 i = 0; i < mesh->mNumFaces; i++)
	{
		if(mesh->mFaces[i].mNumIndices > 3)
		{
			std::cerr << "warning: at AiMeshParser::parse(), face is not triangle, ignoring" << std::endl;
			continue;
		}

		const auto i0 = mesh->mFaces[i].mIndices[0];
		const auto i1 = mesh->mFaces[i].mIndices[1];
		const auto i2 = mesh->mFaces[i].mIndices[2];

		if(mesh->HasPositions())
		{
			Vector3R v0(static_cast<real>(mesh->mVertices[i0].x), static_cast<real>(mesh->mVertices[i0].y), static_cast<real>(mesh->mVertices[i0].z));
			Vector3R v1(static_cast<real>(mesh->mVertices[i1].x), static_cast<real>(mesh->mVertices[i1].y), static_cast<real>(mesh->mVertices[i1].z));
			Vector3R v2(static_cast<real>(mesh->mVertices[i2].x), static_cast<real>(mesh->mVertices[i2].y), static_cast<real>(mesh->mVertices[i2].z));
			
			GTriangle triangle(v0, v1, v2);

			if(mesh->HasNormals())
			{
				Vector3R n0(static_cast<real>(mesh->mNormals[i0].x), static_cast<real>(mesh->mNormals[i0].y), static_cast<real>(mesh->mNormals[i0].z));
				Vector3R n1(static_cast<real>(mesh->mNormals[i1].x), static_cast<real>(mesh->mNormals[i1].y), static_cast<real>(mesh->mNormals[i1].z));
				Vector3R n2(static_cast<real>(mesh->mNormals[i2].x), static_cast<real>(mesh->mNormals[i2].y), static_cast<real>(mesh->mNormals[i2].z));
				n0.normalizeLocal();
				n1.normalizeLocal();
				n2.normalizeLocal();
				triangle.setNa(n0);
				triangle.setNb(n1);
				triangle.setNc(n2);
			}

			if(mesh->HasTextureCoords(0))
			{
				Vector3R uvwA(static_cast<real>(mesh->mTextureCoords[0][i0].x), static_cast<real>(mesh->mTextureCoords[0][i0].y), static_cast<real>(mesh->mTextureCoords[0][i0].z));
				Vector3R uvwB(static_cast<real>(mesh->mTextureCoords[0][i1].x), static_cast<real>(mesh->mTextureCoords[0][i1].y), static_cast<real>(mesh->mTextureCoords[0][i1].z));
				Vector3R uvwC(static_cast<real>(mesh->mTextureCoords[0][i2].x), static_cast<real>(mesh->mTextureCoords[0][i2].y), static_cast<real>(mesh->mTextureCoords[0][i2].z));
				triangle.setUVWa(uvwA);
				triangle.setUVWb(uvwB);
				triangle.setUVWc(uvwC);
			}

			triangleMesh->addTriangle(triangle);
		}
	}// end for each face

	return triangleMesh;
}

}// end namespace ph