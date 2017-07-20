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

ExitStatus LightSource::ciExecute(const std::shared_ptr<LightSource>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph