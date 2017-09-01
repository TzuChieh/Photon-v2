#include "Actor/Material/Material.h"

namespace ph
{

Material::Material() = default;

Material::~Material() = default;

// command interface

SdlTypeInfo Material::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "material");
}

void Material::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph