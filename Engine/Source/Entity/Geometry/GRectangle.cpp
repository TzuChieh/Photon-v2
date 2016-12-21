#include "Entity/Geometry/GRectangle.h"
#include "Core/Primitive/PTriangle.h"
#include "Math/Vector3f.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Entity/Entity.h"
#include "Core/Primitive/PrimitiveStorage.h"

#include <iostream>

namespace ph
{

GRectangle::GRectangle(const float32 width, const float32 height) :
	m_width(width), m_height(height)
{

}

GRectangle::~GRectangle() = default;

void GRectangle::discretize(PrimitiveStorage* const out_data, const Entity& parentEntity) const
{
	if(m_width <= 0.0f || m_height <= 0.0f)
	{
		std::cerr << "warning: GRectangle's dimension is zero or negative" << std::endl;
	}

	const float32 halfWidth = m_width * 0.5f;
	const float32 halfHeight = m_height * 0.5f;

	const Vector3f vA(-halfWidth,  halfHeight, 0.0f);
	const Vector3f vB(-halfWidth, -halfHeight, 0.0f);
	const Vector3f vC( halfWidth, -halfHeight, 0.0f);
	const Vector3f vD( halfWidth,  halfHeight, 0.0f);

	auto metadata = std::make_unique<PrimitiveMetadata>();
	metadata->m_material      = parentEntity.getMaterial();
	metadata->m_localToWorld  = parentEntity.getLocalToWorldTransform();
	metadata->m_worldToLocal  = parentEntity.getWorldToLocalTransform();
	metadata->m_textureMapper = parentEntity.getTextureMapper();

	// 2 triangles for a rectangle (both CCW)
	out_data->add(std::make_unique<PTriangle>(metadata.get(), vA, vB, vD));
	out_data->add(std::make_unique<PTriangle>(metadata.get(), vB, vC, vD));

	out_data->add(std::move(metadata));
}

}// end namespace ph