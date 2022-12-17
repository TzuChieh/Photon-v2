#pragma once

#include "RenderCore/ghi_enums.h"

#include <Common/assertion.h>

#include <type_traits>

namespace ph::editor
{

inline std::size_t num_bytes(const EGHIInfoPixelFormat format)
{
	switch(format)
	{
	case EGHIInfoPixelFormat::Empty: return 0;
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

inline std::size_t num_bytes(const EGHIInfoPixelComponent component)
{
	switch(component)
	{
	case EGHIInfoPixelComponent::Empty: return 0;
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

inline std::size_t num_bytes(const EGHIInfoStorageElement element)
{
	switch(element)
	{
	case EGHIInfoStorageElement::Empty: return 0;
	case EGHIInfoStorageElement::Int8: return 1;
	case EGHIInfoStorageElement::UInt8: return 1;
	case EGHIInfoStorageElement::Int16: return 2;
	case EGHIInfoStorageElement::UInt16: return 2;
	case EGHIInfoStorageElement::Int32: return 4;
	case EGHIInfoStorageElement::UInt32: return 4;
	case EGHIInfoStorageElement::Int64: return 8;
	case EGHIInfoStorageElement::UInt64: return 8;
	case EGHIInfoStorageElement::Float16: return 2;
	case EGHIInfoStorageElement::Float32: return 4;
	case EGHIInfoStorageElement::Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

template<CIsEnum DstEnumType>
DstEnumType translate_to(const EPicturePixelComponent pictureComponent)
{
	if constexpr(std::is_same_v<DstEnumType, EGHIInfoPixelComponent>)
	{
		switch(pictureComponent)
		{
		case EPicturePixelComponent::Empty: return EGHIInfoPixelComponent::Empty;
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
		default: PH_ASSERT_UNREACHABLE_SECTION(); return EGHIInfoPixelComponent::Empty;
		}
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return static_cast<DstEnumType>(0);
	}
}

}// end namespace ph::editor
