#include "RenderCore/GHI.h"
#include "RenderCore/ghi_infos.h"

namespace ph::editor::ghi
{

PH_DEFINE_LOG_GROUP(GHI, GHI);

std::string GHI::toString(const EGraphicsAPI apiType)
{
	switch(apiType)
	{
	case EGraphicsAPI::Unknown: return "Unknown";
	case EGraphicsAPI::OpenGL:  return "OpenGL";

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return "Unknown";
	}
}

GHI::GHI(const EGraphicsAPI apiType)
	: m_apiType(apiType)
{}

GHI::~GHI() = default;

DeviceCapabilityInfo GHI::getDeviceCapabilities()
{
	return DeviceCapabilityInfo{};
}

}// end namespace ph::editor::ghi
