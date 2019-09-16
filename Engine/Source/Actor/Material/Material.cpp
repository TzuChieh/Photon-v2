#include "Actor/Material/Material.h"

namespace ph
{

// command interface

Material::Material(const InputPacket& packet)
{}

SdlTypeInfo Material::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "material");
}

void Material::ciRegister(CommandRegister& cmdRegister) 
{}

}// end namespace ph
