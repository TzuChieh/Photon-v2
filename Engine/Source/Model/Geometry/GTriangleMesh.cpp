#include "Model/Geometry/GTriangleMesh.h"

namespace ph
{

GTriangleMesh::~GTriangleMesh()
{

}

void GTriangleMesh::genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const
{
	for(const auto& gTriangle : m_gTriangles)
	{
		gTriangle->genPrimitives(out_primitives, parentModel);
	}
}

void GTriangleMesh::addTriangle(const std::shared_ptr<GTriangle>& triangle)
{
	m_gTriangles.push_back(triangle);
}

}// end namespace ph