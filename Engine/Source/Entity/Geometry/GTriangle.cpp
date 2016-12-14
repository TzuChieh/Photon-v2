#include "Entity/Geometry/GTriangle.h"
#include "Entity/Geometry/Triangle.h"
#include "Entity/Entity.h"
#include "Entity/TextureMapper/TextureMapper.h"

namespace ph
{

GTriangle::GTriangle(const Vector3f& vA, const Vector3f& vB, const Vector3f& vC) : 
	m_vA(vA), m_vB(vB), m_vC(vC)
{
	const Vector3f faceN = vB.sub(vA).cross(vC.sub(vA)).normalizeLocal();
	m_nA = faceN;
	m_nB = faceN;
	m_nC = faceN;

	m_uvwA.set(0.0f);
	m_uvwB.set(0.0f);
	m_uvwC.set(0.0f);
}

GTriangle::~GTriangle() = default;

void GTriangle::discretize(std::vector<Triangle>* const out_triangles, const Entity* const parentEntity) const
{
	const auto* const textureMapper = parentEntity->getTextureMapper();

	Triangle triangle(parentEntity, m_vA, m_vB, m_vC);

	triangle.setNa(m_nA);
	triangle.setNb(m_nB);
	triangle.setNc(m_nC);

	Vector3f mappedUVW;

	textureMapper->map(m_vA, m_uvwA, &mappedUVW);
	triangle.setUVWa(mappedUVW);

	textureMapper->map(m_vB, m_uvwB, &mappedUVW);
	triangle.setUVWb(mappedUVW);

	textureMapper->map(m_vC, m_uvwC, &mappedUVW);
	triangle.setUVWc(mappedUVW);

	out_triangles->push_back(triangle);
}

}// end namespace ph