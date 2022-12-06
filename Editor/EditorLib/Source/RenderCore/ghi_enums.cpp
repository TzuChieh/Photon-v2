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
	case EGHIInfoPixelComponent::Int8: return 1;
	case EGHIInfoPixelComponent::UInt8: return 1;
	case EGHIInfoPixelComponent::Int16: return 2;
	case EGHIInfoPixelComponent::UInt16: return 2;
	case EGHIInfoPixelComponent::Int32: return 4;
	case EGHIInfoPixelComponent::UInt32: return 4;
	case EGHIInfoPixelComponent::Int64: return 8;
	case EGHIInfoPixelComponent::UInt64: return 8;
	case EGHIInfoPixelComponent::Float16: return 2;
	case EGHIInfoPixelComponent::Float32: return 4;
	case EGHIInfoPixelComponent::Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

EGHIInfoPixelComponent from_picture_pixel_component(const EPicturePixelComponent component)
{
	switch(component)
	{
	case EPicturePixelComponent::Unspecified: return EGHIInfoPixelComponent::Unspecified;
	case EPicturePixelComponent::Int8: return EGHIInfoPixelComponent::Int8;
	case EPicturePixelComponent::UInt8: return EGHIInfoPixelComponent::UInt8;
	case EPicturePixelComponent::Int16: return EGHIInfoPixelComponent::Int16;
	case EPicturePixelComponent::UInt16: return EGHIInfoPixelComponent::UInt16;
	case EPicturePixelComponent::Int32: return EGHIInfoPixelComponent::Int32;
	case EPicturePixelComponent::UInt32: return EGHIInfoPixelComponent::UInt32;
	case EPicturePixelComponent::Int64: return EGHIInfoPixelComponent::Int64;
	case EPicturePixelComponent::UInt64: return EGHIInfoPixelComponent::UInt64;
	case EPicturePixelComponent::Float16: return EGHIInfoPixelComponent::Float16;
	case EPicturePixelComponent::Float32: return EGHIInfoPixelComponent::Float32;
	case EPicturePixelComponent::Float64: return EGHIInfoPixelComponent::Float64;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return EGHIInfoPixelComponent::Unspecified;
	}
}

}// end namespace ph::editor
