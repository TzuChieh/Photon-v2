#pragma once

#include "RenderCore/ghi_enums.h"

#include <Common/assertion.h>

#include <type_traits>

namespace ph::editor::ghi
{

inline std::size_t num_bytes(ESizedPixelFormat sizedFormat)
{
	switch(sizedFormat)
	{
	case ESizedPixelFormat::Empty: return 0;
	case ESizedPixelFormat::RGB_8: return 3 * 1;
	case ESizedPixelFormat::RGBA_8: return 4 * 1;
	case ESizedPixelFormat::RGB_16F: return 3 * 2;
	case ESizedPixelFormat::RGBA_16F: return 4 * 2;
	case ESizedPixelFormat::RGB_32F: return 3 * 4;
	case ESizedPixelFormat::RGBA_32F: return 4 * 4;
	case ESizedPixelFormat::Depth_24_Stencil_8: return 3 + 1;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

inline std::size_t num_bytes(EPixelComponent component)
{
	switch(component)
	{
	case EPixelComponent::Empty: return 0;
	case EPixelComponent::Int8: return 1;
	case EPixelComponent::UInt8: return 1;
	case EPixelComponent::Int16: return 2;
	case EPixelComponent::UInt16: return 2;
	case EPixelComponent::Int32: return 4;
	case EPixelComponent::UInt32: return 4;
	case EPixelComponent::Int64: return 8;
	case EPixelComponent::UInt64: return 8;
	case EPixelComponent::Float16: return 2;
	case EPixelComponent::Float32: return 4;
	case EPixelComponent::Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

inline std::size_t num_bytes(EStorageElement element)
{
	switch(element)
	{
	case EStorageElement::Empty: return 0;
	case EStorageElement::Int8: return 1;
	case EStorageElement::UInt8: return 1;
	case EStorageElement::Int16: return 2;
	case EStorageElement::UInt16: return 2;
	case EStorageElement::Int32: return 4;
	case EStorageElement::UInt32: return 4;
	case EStorageElement::Int64: return 8;
	case EStorageElement::UInt64: return 8;
	case EStorageElement::Float16: return 2;
	case EStorageElement::Float32: return 4;
	case EStorageElement::Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

template<CEnum DstEnumType>
DstEnumType translate_to(EPicturePixelComponent pictureComponent)
{
	if constexpr(std::is_same_v<DstEnumType, EPixelComponent>)
	{
		switch(pictureComponent)
		{
		case EPicturePixelComponent::Empty: return EPixelComponent::Empty;
		case EPicturePixelComponent::Int8: return EPixelComponent::Int8;
		case EPicturePixelComponent::UInt8: return EPixelComponent::UInt8;
		case EPicturePixelComponent::Int16: return EPixelComponent::Int16;
		case EPicturePixelComponent::UInt16: return EPixelComponent::UInt16;
		case EPicturePixelComponent::Int32: return EPixelComponent::Int32;
		case EPicturePixelComponent::UInt32: return EPixelComponent::UInt32;
		case EPicturePixelComponent::Int64: return EPixelComponent::Int64;
		case EPicturePixelComponent::UInt64: return EPixelComponent::UInt64;
		case EPicturePixelComponent::Float16: return EPixelComponent::Float16;
		case EPicturePixelComponent::Float32: return EPixelComponent::Float32;
		case EPicturePixelComponent::Float64: return EPixelComponent::Float64;
		default: PH_ASSERT_UNREACHABLE_SECTION(); return EPixelComponent::Empty;
		}
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return static_cast<DstEnumType>(0);
	}
}

}// end namespace ph::editor::ghi
