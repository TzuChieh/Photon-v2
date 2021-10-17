#pragma once

#include "Frame/TFrame.h"
#include "Common/primitive_type.h"
#include "Math/Color/color_basics.h"
#include "Math/TVector2.h"

#include <cstddef>

namespace ph
{

enum class EPicturePixelFormat
{
	UNSPECIFIED = 0,

	PPF_Grayscale_8,
	PPF_RGB_8,
	PPF_RGBA_8,

	PPF_Grayscale_16F,
	PPF_RGB_16F,
	PPF_RGBA_16F,

	PPF_Grayscale_32F,
	PPF_RGB_32F,
	PPF_RGBA_32F
};

struct RegularPicture final
{
	using Frame = TFrame<float32, 4>;
	using Pixel = Frame::Pixel;

	EPicturePixelFormat nativeFormat;
	math::EColorSpace   colorSpace;
	Frame               frame;

	RegularPicture();

	explicit RegularPicture(math::TVector2<uint32> sizePx);

	RegularPicture(
		math::TVector2<uint32> sizePx,
		EPicturePixelFormat    nativeFormat);

	RegularPicture(
		math::TVector2<uint32> sizePx,
		EPicturePixelFormat    nativeFormat,
		math::EColorSpace      colorSpace);

	std::size_t numComponents() const;
	bool hasAlpha() const;
	bool isLDR() const;
	bool isHDR() const;
};

}// end namespace ph
