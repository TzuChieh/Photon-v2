#include "Actor/Geometry/Geometry.h"
#include "Actor/UvwGenerator/DefaultGenerator.h"

#include <iostream>

namespace ph
{

Geometry::Geometry() : 
	m_uvwGenerator(std::make_shared<DefaultGenerator>())
{

}

Geometry::~Geometry() = default;

void Geometry::setUvwGenerator(const std::shared_ptr<UvwGenerator>& uvwGenerator)
{
	m_uvwGenerator = uvwGenerator;
}

const UvwGenerator* Geometry::getUvwGenerator() const
{
	return m_uvwGenerator.get();
}

std::shared_ptr<Geometry> Geometry::genTransformApplied(const StaticTransform& transform) const
{
	return nullptr;
}

// command interface

SdlTypeInfo Geometry::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "geometry");
}

void Geometry::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph