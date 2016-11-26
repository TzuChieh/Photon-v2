#include "Model/Geometry/GTriangle.h"
#include "Model/Geometry/Triangle.h"
#include "Model/Model.h"
#include "Model/TextureMapper/TextureMapper.h"

namespace ph
{

GTriangle::GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC) : 
	m_vA(vA), m_vB(vB), m_vC(vC)
{

}

GTriangle::~GTriangle() = default;

void GTriangle::discretize(std::vector<Triangle>* const out_triangles, const Model* const parentModel) const
{
	const auto* const textureMapper = parentModel->getTextureMapper();

	Triangle triangle(parentModel, m_vA, m_vB, m_vC);
	/*triangle.setNa(vA.normalize());
	triangle.setNb(vB.normalize());
	triangle.setNc(vC.normalize());*/

	Vector3f mappedUVW;

	textureMapper->map(m_vA, triangle.getUVWa(), &mappedUVW);
	triangle.setUVWa(mappedUVW);

	textureMapper->map(m_vB, triangle.getUVWa(), &mappedUVW);
	triangle.setUVWb(mappedUVW);

	textureMapper->map(m_vC, triangle.getUVWa(), &mappedUVW);
	triangle.setUVWc(mappedUVW);

	out_triangles->push_back(triangle);
}

}// end namespace ph