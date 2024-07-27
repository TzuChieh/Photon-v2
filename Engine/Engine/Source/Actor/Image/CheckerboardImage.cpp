#include "Actor/Image/CheckerboardImage.h"
#include "Core/Texture/TCheckerboardTexture.h"
#include "Actor/Image/ConstantImage.h"

#include <Common/logging.h>

#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CheckerboardImage, Image);

std::shared_ptr<TTexture<Image::ArrayType>> CheckerboardImage::genNumericTexture(
	const CookingContext& ctx)
{
	auto [odd, even] = getOddAndEvenImages();

	return std::make_shared<TCheckerboardTexture<Image::ArrayType>>(
		m_numTilesU,
		m_numTilesV,
		odd->genNumericTexture(ctx),
		even->genNumericTexture(ctx));
}

std::shared_ptr<TTexture<math::Spectrum>> CheckerboardImage::genColorTexture(
	const CookingContext& ctx)
{
	auto [odd, even] = getOddAndEvenImages();

	return std::make_shared<TCheckerboardTexture<math::Spectrum>>(
		m_numTilesU,
		m_numTilesV,
		odd->genColorTexture(ctx),
		even->genColorTexture(ctx));
}

void CheckerboardImage::setNumTiles(real numTilesU, real numTilesV)
{
	constexpr real minNumTiles = 0.0001_r;

	if(numTilesU < minNumTiles || numTilesV < minNumTiles)
	{
		PH_LOG(CheckerboardImage, Note,
			"Setting checkerboard image with number of tiles less than {} is not recommended; "
			"clamping it to minimum size.", minNumTiles);

		numTilesU = std::max(numTilesU, minNumTiles);
		numTilesV = std::max(numTilesV, minNumTiles);
	}

	m_numTilesU = numTilesU;
	m_numTilesV = numTilesV;
}

void CheckerboardImage::setOddImage(std::shared_ptr<Image> oddImage)
{
	m_oddImage = std::move(oddImage);
}

void CheckerboardImage::setEvenImage(std::shared_ptr<Image> evenImage)
{
	m_evenImage = std::move(evenImage);
}

auto CheckerboardImage::getOddAndEvenImages() const
	-> std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>
{
	auto oddImage = m_oddImage;
	auto evenImage = m_evenImage;

	// Set to fallback image if missing
	if(!oddImage || !evenImage)
	{
		PH_LOG(CheckerboardImage, Warning,
			"Missing some required images: image for odd tile ({}), image for even tile ({}); "
			"marking missing images as purple.",
			oddImage ? "available" : "missing", evenImage ? "available" : "missing");

		auto fallbackImage = TSdl<ConstantImage>::makeResource();
		fallbackImage->setColor(math::Vector3R(1, 0, 1), math::EColorSpace::Linear_sRGB);

		if(!oddImage)
		{
			oddImage = fallbackImage;
		}

		if(!evenImage)
		{
			evenImage = fallbackImage;
		}
	}

	PH_ASSERT(oddImage);
	PH_ASSERT(evenImage);
	return {oddImage, evenImage};
}

}// end namespace ph
