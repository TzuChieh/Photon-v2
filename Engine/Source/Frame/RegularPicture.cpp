#include "Frame/RegularPicture.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

RegularPicture::RegularPicture()
	: m_nativeFormat(EPicturePixelFormat::Unspecified)
	, m_colorSpace(math::EColorSpace::Unspecified)
	, m_isReversedComponents(false)
	, m_pictureData()
{}

RegularPicture::RegularPicture(
	const math::Vector2S sizePx,
	const std::size_t numComponents,
	const EPicturePixelComponent componentType)

	: RegularPicture()
{
	m_pictureData = PictureData(sizePx, numComponents, componentType);
}

void RegularPicture::setNativeFormat(const EPicturePixelFormat format)
{
	m_nativeFormat = format;

	// Guess the color space base on LDR/HDR (only if not specified already)
	if(m_colorSpace == math::EColorSpace::Unspecified)
	{
		if(isLDR(format))
		{
			m_colorSpace = math::EColorSpace::sRGB;
		}
		else if(isHDR(format))
		{
			m_colorSpace = math::EColorSpace::Linear_sRGB;
		}
	}
}

void RegularPicture::setComponentReversed(const bool isReversed)
{
	m_isReversedComponents = isReversed;
}

void RegularPicture::setColorSpace(const math::EColorSpace colorSpace)
{
	m_colorSpace = colorSpace;
}

std::size_t RegularPicture::numComponents() const
{
	return numComponents(m_nativeFormat);
}

bool RegularPicture::hasAlpha() const
{
	return hasAlpha(m_nativeFormat);
}

bool RegularPicture::isLDR() const
{
	return isLDR(m_nativeFormat);
}

bool RegularPicture::isHDR() const
{
	return isHDR(m_nativeFormat);
}

bool RegularPicture::isLDR(const EPicturePixelFormat format)
{
	switch(format)
	{
	case EPicturePixelFormat::PPF_Grayscale_8:
	case EPicturePixelFormat::PPF_RGB_8:
	case EPicturePixelFormat::PPF_RGBA_8:
		return true;

	default:
		return false;
	}
}

bool RegularPicture::isHDR(const EPicturePixelFormat format)
{
	switch(format)
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

std::size_t RegularPicture::numComponents(const EPicturePixelFormat format)
{
	switch(format)
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

bool RegularPicture::hasAlpha(const EPicturePixelFormat format)
{
	switch(format)
	{
	case EPicturePixelFormat::PPF_RGBA_8:
	case EPicturePixelFormat::PPF_RGBA_16F:
	case EPicturePixelFormat::PPF_RGBA_32F:
		return true;

	default:
		return false;
	}
}

}// end namespace ph
