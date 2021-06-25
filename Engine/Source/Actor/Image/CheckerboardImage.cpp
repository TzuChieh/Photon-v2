#include "Actor/Image/CheckerboardImage.h"
#include "Core/Texture/TCheckerboardTexture.h"

#include <algorithm>

namespace ph
{

CheckerboardImage::CheckerboardImage() : 
	Image(),
	m_numTilesX(2), m_numTilesY(2)
{}

std::shared_ptr<TTexture<real>> CheckerboardImage::genTextureReal(CookingContext& context) const
{
	const auto& images = checkOutImages();
	if(images.first == nullptr || images.second == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<TCheckerboardTexture<real>>(
		m_numTilesX, m_numTilesY, 
		images.first->genTextureReal(context), 
		images.second->genTextureReal(context));
}

std::shared_ptr<TTexture<math::Vector3R>> CheckerboardImage::genTextureVector3R(
	CookingContext& context) const
{
	const auto& images = checkOutImages();
	if(images.first == nullptr || images.second == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<TCheckerboardTexture<math::Vector3R>>(
		m_numTilesX, m_numTilesY,
		images.first->genTextureVector3R(context), 
		images.second->genTextureVector3R(context));
}

std::shared_ptr<TTexture<Spectrum>> CheckerboardImage::genTextureSpectral(
	CookingContext& context) const
{
	const auto& images = checkOutImages();
	if(images.first == nullptr || images.second == nullptr)
	{
		return nullptr;
	}

	return std::make_shared<TCheckerboardTexture<Spectrum>>(
		m_numTilesX, m_numTilesY,
		images.first->genTextureSpectral(context), 
		images.second->genTextureSpectral(context));
}

void CheckerboardImage::setNumTiles(const real numTilesX, const real numTilesY)
{
	const real minNumTiles = 0.0001_r;

	if(numTilesX < minNumTiles || numTilesY < minNumTiles)
	{
		std::cout << "NOTE: Setting checkerboard image with number of tiles less than "
		          << minNumTiles
		          << " is not recommended; clamping it to minimum size." << std::endl;
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
	auto oddImage  = m_oddImage.lock();
	auto evenImage = m_evenImage.lock();
	if(oddImage == nullptr || evenImage == nullptr)
	{
		std::cerr << "warning: at CheckerboardImage::checkOutImages(), "
		          << "some required image is empty" << std::endl;
	}

	return {std::move(oddImage), std::move(evenImage)};
}

}// end namespace ph
