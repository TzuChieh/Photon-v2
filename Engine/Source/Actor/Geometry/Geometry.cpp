#include "Actor/Geometry/Geometry.h"
#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Common/assertion.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <iostream>

namespace ph
{

Geometry::Geometry() : 
	m_uvwMapper(std::make_shared<SphericalMapper>())
{
	PH_ASSERT(m_uvwMapper);
}

void Geometry::setUvwMapper(const std::shared_ptr<UvwMapper>& uvwMapper)
{
	PH_ASSERT(uvwMapper);

	m_uvwMapper = uvwMapper;
}

const UvwMapper* Geometry::getUvwMapper() const
{
	PH_ASSERT(m_uvwMapper);

	return m_uvwMapper.get();
}

// command interface

SdlTypeInfo Geometry::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "geometry");
}

void Geometry::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
