#include "RenderCore/GHI.h"
#include "RenderCore/GHIInfoDeviceCapability.h"

namespace ph::editor
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

GHIInfoDeviceCapability GHI::getDeviceCapabilities()
{
	return GHIInfoDeviceCapability();
}

}// end namespace ph::editor
