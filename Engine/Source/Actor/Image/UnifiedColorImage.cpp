#include "Actor/Image/UnifiedColorImage.h"
#include "Actor/Image/ConstantImage.h"
#include "Actor/Image/RasterFileImage.h"

#include <utility>

namespace ph
{

std::shared_ptr<TTexture<Image::ArrayType>> UnifiedColorImage::genNumericTexture(
	const CookingContext& ctx)
{
	auto outputImage = getOutputImage();
	if(outputImage)
	{
		return outputImage->genNumericTexture(ctx);
	}
	else
	{
		return ConstantImage(getConstant()).genNumericTexture(ctx);
	}
}

std::shared_ptr<TTexture<math::Spectrum>> UnifiedColorImage::genColorTexture(
	const CookingContext& ctx)
{
	auto outputImage = getOutputImage();
	if(outputImage)
	{
		return outputImage->genColorTexture(ctx);
	}
	else
	{
		return ConstantImage(getConstant(), getConstantColorSpace()).genColorTexture(ctx);
	}
}

bool UnifiedColorImage::isInlinable() const
{
	return !m_image;
}

UnifiedColorImage& UnifiedColorImage::setImage(std::shared_ptr<Image> image)
{
	m_image = std::move(image);
	return *this;
}

UnifiedColorImage& UnifiedColorImage::setFile(Path imageFile)
{
	m_imageFile.setPath(std::move(imageFile));
	return *this;
}

UnifiedColorImage& UnifiedColorImage::setFile(ResourceIdentifier imageFile)
{
	m_imageFile = std::move(imageFile);
	return *this;
}

UnifiedColorImage& UnifiedColorImage::setConstant(math::Vector3R constant)
{
	m_constant = std::move(constant);
	m_constantColorSpace = math::EColorSpace::Unspecified;
	return *this;
}

UnifiedColorImage& UnifiedColorImage::setConstantColor(math::Vector3R colorValue, math::EColorSpace colorSpace)
{
	m_constant = std::move(colorValue);
	m_constantColorSpace = colorSpace;
	return *this;
}

Image* UnifiedColorImage::getImage() const
{
	return m_image.get();
}

const ResourceIdentifier& UnifiedColorImage::getFile() const
{
	return m_imageFile;
}

math::Vector3R UnifiedColorImage::getConstant() const
{
	return m_constant;
}

math::EColorSpace UnifiedColorImage::getConstantColorSpace() const
{
	return m_constantColorSpace;
}

std::shared_ptr<Image> UnifiedColorImage::getOutputImage() const
{
	if(m_image)
	{
		return m_image;
	}

	if(!m_imageFile.getPath().isEmpty())
	{
		auto rasterImage = TSdl<RasterFileImage>::makeResource();
		rasterImage->setFilePath(m_imageFile.getPath());
		return rasterImage;
	}

	return nullptr;
}

}// end namespace ph
