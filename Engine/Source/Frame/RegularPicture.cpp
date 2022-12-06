#include "Frame/RegularPicture.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

RegularPictureFormat::RegularPictureFormat()
	: m_colorSpace(math::EColorSpace::sRGB)
	, m_isReversedComponents(false)
	, m_hasAlpha(false)
	, m_isGrayscale(false)
{}

void RegularPictureFormat::setColorSpace(const math::EColorSpace colorSpace)
{
	m_colorSpace = colorSpace;
}

void RegularPictureFormat::setIsReversedComponents(const bool isReversedComponents)
{
	m_isReversedComponents = isReversedComponents;
}

void RegularPictureFormat::setHasAlpha(const bool hasAlpha)
{
	m_hasAlpha = hasAlpha;
}

void RegularPictureFormat::setIsGrayscale(const bool isGrayscale)
{
	m_isGrayscale = isGrayscale;
}

RegularPicture::RegularPicture()
	: m_format()
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

RegularPicture::RegularPicture(RegularPicture&& other) = default;

void RegularPicture::setFormat(const RegularPictureFormat& format)
{
	m_format = format;
}

bool RegularPicture::isLDR() const
{
	return isLDR(m_pictureData.getComponentType());
}

bool RegularPicture::isHDR() const
{
	return isHDR(m_pictureData.getComponentType());
}

RegularPicture& RegularPicture::operator = (RegularPicture&& rhs) = default;

bool RegularPicture::isLDR(const EPicturePixelComponent componentType)
{
	switch(componentType)
	{
	case EPicturePixelComponent::Int8:
	case EPicturePixelComponent::UInt8:
		return true;

	default:
		return false;
	}
}

bool RegularPicture::isHDR(const EPicturePixelComponent componentType)
{
	switch(componentType)
	{
	case EPicturePixelComponent::Int16:
	case EPicturePixelComponent::UInt16:
	case EPicturePixelComponent::Int32:
	case EPicturePixelComponent::UInt32:
	case EPicturePixelComponent::Int64:
	case EPicturePixelComponent::UInt64:
	case EPicturePixelComponent::Float16:
	case EPicturePixelComponent::Float32:
	case EPicturePixelComponent::Float64:
		return true;

	default:
		return false;
	}
}

}// end namespace ph
