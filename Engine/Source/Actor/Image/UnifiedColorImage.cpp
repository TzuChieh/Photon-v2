#include "Actor/Image/UnifiedColorImage.h"
#include "Actor/Image/ConstantImage.h"

#include <utility>

namespace ph
{

UnifiedColorImage::UnifiedColorImage() :

	Image(),

	m_image             (nullptr),
	m_constant          (0),
	m_constantColorSpace(math::EColorSpace::Unspecified)
{}

UnifiedColorImage::UnifiedColorImage(std::shared_ptr<Image> image) :
	UnifiedColorImage()
{
	setImage(std::move(image));
}

UnifiedColorImage::UnifiedColorImage(const math::Vector3R constant) :
	UnifiedColorImage()
{
	setConstant(constant);
}

UnifiedColorImage::UnifiedColorImage(const math::Vector3R colorValue, const math::EColorSpace colorSpace) :
	UnifiedColorImage()
{
	setConstantColor(colorValue, colorSpace);
}

std::shared_ptr<TTexture<Image::ArrayType>> UnifiedColorImage::genNumericTexture(
	const CookingContext& ctx)
{
	if(m_image)
	{
		return m_image->genNumericTexture(ctx);
	}
	else
	{
		return ConstantImage(getConstant()).genNumericTexture(ctx);
	}
}

std::shared_ptr<TTexture<math::Spectrum>> UnifiedColorImage::genColorTexture(
	const CookingContext& ctx)
{
	if(m_image)
	{
		return m_image->genColorTexture(ctx);
	}
	else
	{
		return ConstantImage(getConstant(), getConstantColorSpace()).genColorTexture(ctx);
	}
}

UnifiedColorImage& UnifiedColorImage::setImage(std::shared_ptr<Image> image)
{
	m_image = std::move(image);
	return *this;
}

UnifiedColorImage& UnifiedColorImage::setConstant(math::Vector3R constant)
{
	m_constant           = std::move(constant);
	m_constantColorSpace = math::EColorSpace::Unspecified;
	return *this;
}

UnifiedColorImage& UnifiedColorImage::setConstantColor(math::Vector3R colorValue, const math::EColorSpace colorSpace)
{
	m_constant           = std::move(colorValue);
	m_constantColorSpace = colorSpace;
	return *this;
}

Image* UnifiedColorImage::getImage() const
{
	return m_image.get();
}

math::Vector3R UnifiedColorImage::getConstant() const
{
	return m_constant;
}

math::EColorSpace UnifiedColorImage::getConstantColorSpace() const
{
	return m_constantColorSpace;
}

}// end namespace ph
