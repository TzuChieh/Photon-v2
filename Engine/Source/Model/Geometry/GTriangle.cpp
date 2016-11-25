#include "Model/Geometry/GTriangle.h"
#include "Model/Primitive/PTriangle.h"
#include "Model/Geometry/Triangle.h"

namespace ph
{

GTriangle::GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC) : 
	m_vA(vA), m_vB(vB), m_vC(vC)
{

}

GTriangle::~GTriangle() = default;

void GTriangle::genPrimitives(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const Model* const parentModel) const
{
	out_primitives->push_back(std::make_unique<PTriangle>(this, parentModel));
}

void GTriangle::discretize(std::vector<Triangle>* const out_triangles, const Model* const parentModel) const
{
	out_triangles->push_back(Triangle(parentModel, m_vA, m_vB, m_vC));
}

}// end namespace ph