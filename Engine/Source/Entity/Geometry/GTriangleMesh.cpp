#include "Entity/Geometry/GTriangleMesh.h"
#include "Entity/Geometry/GTriangle.h"
#include "Entity/Primitive/PrimitiveMetadata.h"

namespace ph
{

GTriangleMesh::~GTriangleMesh()
{

}

void GTriangleMesh::discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata* const metadata) const
{
	for(const auto& gTriangle : m_gTriangles)
	{
		gTriangle.discretize(out_primitives, metadata);
	}
}

void GTriangleMesh::addTriangle(const GTriangle gTriangle)
{
	m_gTriangles.push_back(gTriangle);
}

}// end namespace ph