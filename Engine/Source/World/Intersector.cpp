#include "World/Intersector.h"
#include "Entity/Geometry/Triangle.h"

namespace ph
{

Intersector::~Intersector() = default;

void Intersector::clearData()
{
	m_triangles.clear();
	m_triangles.shrink_to_fit();
}

void Intersector::addTriangle(const Triangle& triangle)
{
	m_triangles.push_back(triangle);
}

}// end namespace ph