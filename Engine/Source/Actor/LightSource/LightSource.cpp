#include "Actor/LightSource/LightSource.h"

namespace ph
{

LightSource::LightSource() = default;

LightSource::~LightSource() = default;

// command interface

SdlTypeInfo LightSource::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_LIGHT_SOURCE, "light-source");
}

void LightSource::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph