#include "Actor/Image/CheckerboardImage.h"
#include "Core/Texture/TCheckerboardTexture.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CheckerboardImage, Image);

CheckerboardImage::CheckerboardImage() : 
	Image(),
	m_numTilesX(2), m_numTilesY(2)
{}

std::shared_ptr<TTexture<Image::ArrayType>> CheckerboardImage::genNumericTexture(
	const CookingContext& ctx)
{
	const auto& images = checkOutImages();
	if(images.first == nullptr || images.second == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<TCheckerboardTexture<Image::ArrayType>>(
		m_numTilesX, m_numTilesY,
		images.first->genNumericTexture(ctx),
		images.second->genNumericTexture(ctx));
}

std::shared_ptr<TTexture<math::Spectrum>> CheckerboardImage::genColorTexture(
	const CookingContext& ctx)
{
	const auto& images = checkOutImages();
	if(images.first == nullptr || images.second == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<TCheckerboardTexture<math::Spectrum>>(
		m_numTilesX, m_numTilesY,
		images.first->genColorTexture(ctx),
		images.second->genColorTexture(ctx));
}

void CheckerboardImage::setNumTiles(const real numTilesX, const real numTilesY)
{
	constexpr real minNumTiles = 0.0001_r;

	if(numTilesX < minNumTiles || numTilesY < minNumTiles)
	{
		PH_LOG(CheckerboardImage, Note,
			"Setting checkerboard image with number of tiles less than {} is not recommended; "
			"clamping it to minimum size.", minNumTiles);
	}

	m_numTilesX = std::max(numTilesX, minNumTiles);
	m_numTilesY = std::max(numTilesY, minNumTiles);
}

void CheckerboardImage::setOddImage(const std::weak_ptr<Image>& oddImage)
{
	m_oddImage = oddImage;
}

void CheckerboardImage::setEvenImage(const std::weak_ptr<Image>& evenImage)
{
	m_evenImage = evenImage;
}

auto CheckerboardImage::checkOutImages() const
	-> std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>
{
	std::shared_ptr<Image> oddImage  = m_oddImage.lock();
	std::shared_ptr<Image> evenImage = m_evenImage.lock();
	if(oddImage == nullptr || evenImage == nullptr)
	{
		PH_LOG(CheckerboardImage, Warning,
			"at CheckerboardImage::checkOutImages(), some required image is empty");
	}

	return {oddImage, evenImage};
}

}// end namespace ph
