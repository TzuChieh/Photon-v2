#pragma once

#include "Frame/TFrame.h"
#include "Common/primitive_type.h"
#include "Math/Color/color_basics.h"
#include "Math/TVector2.h"

#include <cstddef>

namespace ph
{

/*! @brief Pixel format of picture.
The characters RGB does not mean the pixel is in RGB color space. It simply represents pixel components, 
using R, G, B as placeholders.
*/
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

class RegularPicture final
{
public:
	using Frame = TFrame<float32, 4>;
	using Pixel = Frame::Pixel;

	/*! @brief Data type of the picture when stored natively (e.g., on disk). */
	EPicturePixelFormat nativeFormat;

	/*! @brief Color space of the loaded picture. */
	math::EColorSpace colorSpace;

	/*! @brief Whether the pixel components of the frame is stored reversely, e.g, in ABGR order. */
	bool isReversedComponents;

	/*! @brief Storage of actual pixel data. Promoted to 32-bit floats. */
	Frame frame;

	// TODO: do we need flag for different alpha order? e.g., ARGB vs RGBA. Do such format exist?

public:
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
