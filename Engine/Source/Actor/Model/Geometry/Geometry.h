#pragma once

#include "Core/Primitive/Primitive.h"

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Primitive;
class PrimitiveStorage;
class TextureMapper;

class Geometry
{
public:
	Geometry();
	virtual ~Geometry() = 0;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const = 0;

	void setTextureMapper(const std::shared_ptr<TextureMapper>& textureMapper);
	const TextureMapper* getTextureMapper() const;

protected:
	std::shared_ptr<TextureMapper> m_textureMapper;
};

}// end namespace ph