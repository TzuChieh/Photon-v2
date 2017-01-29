#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GTriangle.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

namespace ph
{

GTriangleMesh::~GTriangleMesh()
{

}

void GTriangleMesh::discretize(const PrimitiveBuildingMaterial& data,
                               std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	for(const auto& gTriangle : m_gTriangles)
	{
		gTriangle.discretize(data, out_primitives);
	}
}

void GTriangleMesh::addTriangle(const GTriangle gTriangle)
{
	m_gTriangles.push_back(gTriangle);
}

}// end namespace ph