#pragma once

#include "Common/assertion.h"

#include <cstddef>

namespace ph
{

/*! @brief Pixel layout of picture.
Represent the ordering of pixel components with respect to a color space. The characters RGB
does not mean the pixel is in RGB color space. It simply represents pixel components,
using R, G, B as placeholders.
*/
enum class EPicturePixelLayout
{
	Unspecified = 0,

	Grayscale,
	RGB,
	RGBA,
	BGR,
	ABGR
};

enum class EPicturePixelComponent
{
	Unspecified = 0,

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

inline std::size_t num_bytes_in_component(const EPicturePixelComponent componentType)
{
	switch(componentType)
	{
	case EPicturePixelComponent::Unspecified: return 0;
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
