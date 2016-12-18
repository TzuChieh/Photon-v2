#include "Entity/Geometry/GTriangleMesh.h"
#include "Entity/Geometry/GTriangle.h"

namespace ph
{

GTriangleMesh::~GTriangleMesh()
{

}

void GTriangleMesh::discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Entity* const parentEntity) const
{
	for(const auto& gTriangle : m_gTriangles)
	{
		gTriangle.discretize(out_primitives, parentEntity);
	}
}

void GTriangleMesh::addTriangle(const GTriangle gTriangle)
{
	m_gTriangles.push_back(gTriangle);
}

}// end namespace ph