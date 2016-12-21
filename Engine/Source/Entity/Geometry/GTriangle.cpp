#include "Entity/Geometry/GTriangle.h"
#include "Core/Primitive/PTriangle.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Entity/TextureMapper/TextureMapper.h"
#include "Core/Primitive/PrimitiveStorage.h"
#include "Entity/Entity.h"

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

void GTriangle::discretize(PrimitiveStorage* const out_data, const Entity& parentEntity) const
{
	auto metadata = std::make_unique<PrimitiveMetadata>();
	metadata->m_material      = parentEntity.getMaterial();
	metadata->m_localToWorld  = parentEntity.getLocalToWorldTransform();
	metadata->m_worldToLocal  = parentEntity.getWorldToLocalTransform();
	metadata->m_textureMapper = parentEntity.getTextureMapper();

	const auto* const textureMapper = metadata->m_textureMapper;

	PTriangle triangle(metadata.get(), m_vA, m_vB, m_vC);

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

	out_data->add(std::make_unique<PTriangle>(triangle));
	out_data->add(std::move(metadata));
}

}// end namespace ph