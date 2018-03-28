#include "Actor/LightSource/LightSource.h"

namespace ph
{

LightSource::LightSource() = default;

LightSource::~LightSource() = default;

// command interface

LightSource::LightSource(const InputPacket& packet)
{}

SdlTypeInfo LightSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "light-source");
}

void LightSource::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph