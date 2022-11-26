#include "RenderCore/ghi_enums.h"

#include <Common/assertion.h>

namespace ph::editor
{

std::size_t apparent_bytes_in_single_pixel(const EGHIInfoPixelFormat format)
{
	switch(format)
	{
	case EGHIInfoPixelFormat::RGB_8: return 3 * 1;
	case EGHIInfoPixelFormat::RGBA_8: return 4 * 1;
	case EGHIInfoPixelFormat::RGB_16F: return 3 * 2;
	case EGHIInfoPixelFormat::RGBA_16F: return 4 * 2;
	case EGHIInfoPixelFormat::RGB_32F: return 3 * 4;
	case EGHIInfoPixelFormat::RGBA_32F: return 4 * 4;
	case EGHIInfoPixelFormat::Depth_24_Stencil_8: return 3 + 1;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

std::size_t apparent_bytes_in_pixel_component(const EGHIInfoPixelComponent component)
{
	switch(component)
	{
	case EGHIInfoPixelComponent::LDR_8: return 1;
	case EGHIInfoPixelComponent::HDR_16F: return 2;
	case EGHIInfoPixelComponent::HDR_32F: return 4;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

}// end namespace ph::editor
