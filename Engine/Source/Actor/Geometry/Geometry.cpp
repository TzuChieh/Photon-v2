#include "Actor/Geometry/Geometry.h"
#include "Actor/StGenerator/DefaultGenerator.h"

#include <iostream>

namespace ph
{

Geometry::Geometry() : 
	m_stGenerator(std::make_shared<DefaultGenerator>())
{

}

Geometry::~Geometry() = default;

void Geometry::setStGenerator(const std::shared_ptr<StGenerator>& stGenerator)
{
	m_stGenerator = stGenerator;
}

const StGenerator* Geometry::getStGenerator() const
{
	return m_stGenerator.get();
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