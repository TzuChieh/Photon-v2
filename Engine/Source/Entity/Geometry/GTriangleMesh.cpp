#include "Entity/Geometry/GTriangleMesh.h"
#include "Entity/Geometry/Triangle.h"

namespace ph
{

GTriangleMesh::~GTriangleMesh()
{

}

void GTriangleMesh::discretize(std::vector<Triangle>* const out_triangles, const Entity* const parentEntity) const
{
	for(const auto& gTriangle : m_gTriangles)
	{
		gTriangle.discretize(out_triangles, parentEntity);
	}
}

void GTriangleMesh::addTriangle(const GTriangle gTriangle)
{
	m_gTriangles.push_back(gTriangle);
}

}// end namespace ph