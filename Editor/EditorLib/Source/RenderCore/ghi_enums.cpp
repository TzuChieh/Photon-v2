#include "RenderCore/ghi_enums.h"

#include <Common/assertion.h>

namespace ph::editor
{

std::size_t apparent_bytes_in_single_pixel(const EGHIPixelFormat format)
{
	switch(format)
	{
	case EGHIPixelFormat::RGB_8: return 3 * 1;
	case EGHIPixelFormat::RGBA_8: return 4 * 1;
	case EGHIPixelFormat::RGB_16F: return 3 * 2;
	case EGHIPixelFormat::RGBA_16F: return 4 * 2;
	case EGHIPixelFormat::RGB_32F: return 3 * 4;
	case EGHIPixelFormat::RGBA_32F: return 4 * 4;
	case EGHIPixelFormat::Depth_24_Stencil_8: return 3 + 1;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

std::size_t apparent_bytes_in_pixel_component(const EGHIPixelComponent component)
{
	switch(component)
	{
	case EGHIPixelComponent::LDR_8: return 1;
	case EGHIPixelComponent::HDR_16F: return 2;
	case EGHIPixelComponent::HDR_32F: return 4;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

}// end namespace ph::editor
