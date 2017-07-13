#include "Actor/Geometry/Geometry.h"
#include "Actor/TextureMapper/TextureMapper.h"
#include "Actor/TextureMapper/DefaultMapper.h"

#include <iostream>

namespace ph
{

Geometry::Geometry() : 
	m_textureMapper(std::make_shared<DefaultMapper>())
{

}

Geometry::~Geometry() = default;

void Geometry::setTextureMapper(const std::shared_ptr<TextureMapper>& textureMapper)
{
	m_textureMapper = textureMapper;
}

const TextureMapper* Geometry::getTextureMapper() const
{
	return m_textureMapper.get();
}

std::shared_ptr<Geometry> Geometry::genTransformApplied(const Transform* transform) const
{
	return nullptr;
}

// command interface

Geometry::Geometry(const InputPacket& packet) :
	Geometry()
{

}

SdlTypeInfo Geometry::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "geometry");
}

ExitStatus Geometry::ciExecute(const std::shared_ptr<Geometry>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph