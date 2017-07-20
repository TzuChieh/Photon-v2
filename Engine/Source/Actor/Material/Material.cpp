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

ExitStatus Material::ciExecute(const std::shared_ptr<Material>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph