#include "Actor/LightSource/LightSource.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/MatteOpaque.h"

namespace ph
{

std::shared_ptr<Geometry> LightSource::genGeometry(CookingContext& context) const
{
	return nullptr;
}

std::shared_ptr<Material> LightSource::genMaterial(CookingContext& context) const
{
	const Vector3R linearSrgbAlbedo(0.5_r);
	return std::make_shared<MatteOpaque>(linearSrgbAlbedo);
}

// command interface

LightSource::LightSource(const InputPacket& packet)
{}

SdlTypeInfo LightSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "light-source");
}

void LightSource::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph