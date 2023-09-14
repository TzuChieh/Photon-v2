#pragma once

#include "RenderCore/ghi_enums.h"

#include <Common/assertion.h>

#include <type_traits>

namespace ph::editor
{

inline std::size_t num_bytes(EGHISizedPixelFormat sizedFormat)
{
	switch(sizedFormat)
	{
	case EGHISizedPixelFormat::Empty: return 0;
	case EGHISizedPixelFormat::RGB_8: return 3 * 1;
	case EGHISizedPixelFormat::RGBA_8: return 4 * 1;
	case EGHISizedPixelFormat::RGB_16F: return 3 * 2;
	case EGHISizedPixelFormat::RGBA_16F: return 4 * 2;
	case EGHISizedPixelFormat::RGB_32F: return 3 * 4;
	case EGHISizedPixelFormat::RGBA_32F: return 4 * 4;
	case EGHISizedPixelFormat::Depth_24_Stencil_8: return 3 + 1;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

inline std::size_t num_bytes(EGHIPixelComponent component)
{
	switch(component)
	{
	case EGHIPixelComponent::Empty: return 0;
	case EGHIPixelComponent::Int8: return 1;
	case EGHIPixelComponent::UInt8: return 1;
	case EGHIPixelComponent::Int16: return 2;
	case EGHIPixelComponent::UInt16: return 2;
	case EGHIPixelComponent::Int32: return 4;
	case EGHIPixelComponent::UInt32: return 4;
	case EGHIPixelComponent::Int64: return 8;
	case EGHIPixelComponent::UInt64: return 8;
	case EGHIPixelComponent::Float16: return 2;
	case EGHIPixelComponent::Float32: return 4;
	case EGHIPixelComponent::Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

inline std::size_t num_bytes(EGHIStorageElement element)
{
	switch(element)
	{
	case EGHIStorageElement::Empty: return 0;
	case EGHIStorageElement::Int8: return 1;
	case EGHIStorageElement::UInt8: return 1;
	case EGHIStorageElement::Int16: return 2;
	case EGHIStorageElement::UInt16: return 2;
	case EGHIStorageElement::Int32: return 4;
	case EGHIStorageElement::UInt32: return 4;
	case EGHIStorageElement::Int64: return 8;
	case EGHIStorageElement::UInt64: return 8;
	case EGHIStorageElement::Float16: return 2;
	case EGHIStorageElement::Float32: return 4;
	case EGHIStorageElement::Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

template<CEnum DstEnumType>
DstEnumType translate_to(EPicturePixelComponent pictureComponent)
{
	if constexpr(std::is_same_v<DstEnumType, EGHIPixelComponent>)
	{
		switch(pictureComponent)
		{
		case EPicturePixelComponent::Empty: return EGHIPixelComponent::Empty;
		case EPicturePixelComponent::Int8: return EGHIPixelComponent::Int8;
		case EPicturePixelComponent::UInt8: return EGHIPixelComponent::UInt8;
		case EPicturePixelComponent::Int16: return EGHIPixelComponent::Int16;
		case EPicturePixelComponent::UInt16: return EGHIPixelComponent::UInt16;
		case EPicturePixelComponent::Int32: return EGHIPixelComponent::Int32;
		case EPicturePixelComponent::UInt32: return EGHIPixelComponent::UInt32;
		case EPicturePixelComponent::Int64: return EGHIPixelComponent::Int64;
		case EPicturePixelComponent::UInt64: return EGHIPixelComponent::UInt64;
		case EPicturePixelComponent::Float16: return EGHIPixelComponent::Float16;
		case EPicturePixelComponent::Float32: return EGHIPixelComponent::Float32;
		case EPicturePixelComponent::Float64: return EGHIPixelComponent::Float64;
		default: PH_ASSERT_UNREACHABLE_SECTION(); return EGHIPixelComponent::Empty;
		}
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return static_cast<DstEnumType>(0);
	}
}

}// end namespace ph::editor
