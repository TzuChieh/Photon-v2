#include "Entity/Geometry/GTriangleMesh.h"
#include "Entity/Geometry/GTriangle.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Core/Primitive/PrimitiveStorage.h"
#include "Entity/Entity.h"

namespace ph
{

GTriangleMesh::~GTriangleMesh()
{

}

void GTriangleMesh::discretize(PrimitiveStorage* const out_data, const Entity& parentEntity) const
{
	for(const auto& gTriangle : m_gTriangles)
	{
		gTriangle.discretize(out_data, parentEntity);
	}
}

void GTriangleMesh::addTriangle(const GTriangle gTriangle)
{
	m_gTriangles.push_back(gTriangle);
}

}// end namespace ph