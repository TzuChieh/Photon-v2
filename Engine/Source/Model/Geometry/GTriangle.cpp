#include "Model/Geometry/GTriangle.h"
#include "Model/Geometry/Triangle.h"

namespace ph
{

GTriangle::GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC) : 
	m_vA(vA), m_vB(vB), m_vC(vC)
{

}

GTriangle::~GTriangle() = default;

void GTriangle::discretize(std::vector<Triangle>* const out_triangles, const Model* const parentModel) const
{
	out_triangles->push_back(Triangle(parentModel, m_vA, m_vB, m_vC));
}

}// end namespace ph