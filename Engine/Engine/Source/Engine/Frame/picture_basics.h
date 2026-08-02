#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <type_traits>

namespace ph
{

enum class EPicturePixelComponent
{
	Empty = 0,

	Int8,
	UInt8,
	Int16,
	UInt16,
	Int32,
	UInt32,
	Int64,
	UInt64,
	Float16,
	Float32,
	Float64
};

template<typename Component>
inline constexpr EPicturePixelComponent component_type_of()
{
	using PlainComponent = std::remove_cv_t<Component>;
	if constexpr(std::is_same_v<PlainComponent, int8>)
	{
		return EPicturePixelComponent::Int8;
	}
	else if constexpr(std::is_same_v<PlainComponent, uint8>)
	{
		return EPicturePixelComponent::UInt8;
	}
	else if constexpr(std::is_same_v<PlainComponent, int16>)
	{
		return EPicturePixelComponent::Int16;
	}
	else if constexpr(std::is_same_v<PlainComponent, uint16>)
	{
		return EPicturePixelComponent::UInt16;
	}
	else if constexpr(std::is_same_v<PlainComponent, int32>)
	{
		return EPicturePixelComponent::Int32;
	}
	else if constexpr(std::is_same_v<PlainComponent, uint32>)
	{
		return EPicturePixelComponent::UInt32;
	}
	else if constexpr(std::is_same_v<PlainComponent, int64>)
	{
		return EPicturePixelComponent::Int64;
	}
	else if constexpr(std::is_same_v<PlainComponent, uint64>)
	{
		return EPicturePixelComponent::UInt64;
	}
	else if constexpr(std::is_same_v<PlainComponent, float32>)
	{
		return EPicturePixelComponent::Float32;
	}
	else if constexpr(std::is_same_v<PlainComponent, float64>)
	{
		return EPicturePixelComponent::Float64;
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(PlainComponent, "Unsupported picture component type.");
		return EPicturePixelComponent::Empty;
	}
}

inline std::size_t num_bytes_in_component(const EPicturePixelComponent componentType)
{
	switch(componentType)
	{
	case EPicturePixelComponent::Empty: return 0;
	case EPicturePixelComponent::Int8: return 1;
	case EPicturePixelComponent::UInt8: return 1;
	case EPicturePixelComponent::Int16: return 2;
	case EPicturePixelComponent::UInt16: return 2;
	case EPicturePixelComponent::Int32: return 4;
	case EPicturePixelComponent::UInt32: return 4;
	case EPicturePixelComponent::Int64: return 8;
	case EPicturePixelComponent::UInt64: return 8;
	case EPicturePixelComponent::Float16: return 2;
	case EPicturePixelComponent::Float32: return 4;
	case EPicturePixelComponent::Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

}// end namespace ph
