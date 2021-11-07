#include "Frame/pictures.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

RegularPicture::RegularPicture() :

	RegularPicture(
		{0, 0},
		EPicturePixelFormat::UNSPECIFIED)
{}

RegularPicture::RegularPicture(const math::TVector2<uint32> sizePx) :
	RegularPicture(sizePx, EPicturePixelFormat::UNSPECIFIED)
{}

RegularPicture::RegularPicture(
	const math::TVector2<uint32> sizePx,
	const EPicturePixelFormat    nativeFormat) :

	RegularPicture(
		sizePx, 
		nativeFormat,
		math::EColorSpace::UNSPECIFIED)
{
	// Guess the color space base on LDR/HDR
	if(isLDR())
	{
		colorSpace = math::EColorSpace::sRGB;
	}
	else if(isHDR())
	{
		colorSpace = math::EColorSpace::Linear_sRGB;
	}
}

RegularPicture::RegularPicture(
	const math::TVector2<uint32> sizePx,
	const EPicturePixelFormat    nativeFormat,
	const math::EColorSpace      colorSpace) :

	nativeFormat        (nativeFormat),
	colorSpace          (colorSpace),
	frame               (sizePx.x(), sizePx.y()),
	isReversedComponents(false)
{}

std::size_t RegularPicture::numComponents() const
{
	switch(nativeFormat)
	{
	case EPicturePixelFormat::PPF_Grayscale_8:
	case EPicturePixelFormat::PPF_Grayscale_16F:
	case EPicturePixelFormat::PPF_Grayscale_32F:
		return 1;

	case EPicturePixelFormat::PPF_RGB_8: 
	case EPicturePixelFormat::PPF_RGB_16F:
	case EPicturePixelFormat::PPF_RGB_32F:
		return 3;

	case EPicturePixelFormat::PPF_RGBA_8:
	case EPicturePixelFormat::PPF_RGBA_16F:
	case EPicturePixelFormat::PPF_RGBA_32F:
		return 4;
		
	default:
		return 0;
	}
}

bool RegularPicture::hasAlpha() const
{
	switch(nativeFormat)
	{
	case EPicturePixelFormat::PPF_RGBA_8:
	case EPicturePixelFormat::PPF_RGBA_16F:
	case EPicturePixelFormat::PPF_RGBA_32F:
		return true;

	default:
		return false;
	}
}

bool RegularPicture::isLDR() const
{
	switch(nativeFormat)
	{
	case EPicturePixelFormat::PPF_Grayscale_8:
	case EPicturePixelFormat::PPF_RGB_8:
	case EPicturePixelFormat::PPF_RGBA_8:
		return true;

	default:
		return false;
	}
}

bool RegularPicture::isHDR() const
{
	switch(nativeFormat)
	{
	case EPicturePixelFormat::PPF_Grayscale_16F:
	case EPicturePixelFormat::PPF_RGB_16F:
	case EPicturePixelFormat::PPF_RGBA_16F:
	case EPicturePixelFormat::PPF_Grayscale_32F:
	case EPicturePixelFormat::PPF_RGB_32F:
	case EPicturePixelFormat::PPF_RGBA_32F:
		return true;

	default:
		return false;
	}
}

}// end namespace ph
