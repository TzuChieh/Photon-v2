#include "Actor/Model/Geometry/GTriangleMesh.h"
#include "Actor/Model/Geometry/GTriangle.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Core/Primitive/PrimitiveStorage.h"
#include "Actor/Model/Model.h"

namespace ph
{

GTriangleMesh::~GTriangleMesh()
{

}

void GTriangleMesh::discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const
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