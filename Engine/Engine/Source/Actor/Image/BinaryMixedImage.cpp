#include "Actor/Image/BinaryMixedImage.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(BinaryMixedImage, Image);

BinaryMixedImage::BinaryMixedImage() :

	Image(),

	m_imageA(),
	m_imageB()
{}

auto BinaryMixedImage::checkoutImages() const
	-> std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>
{
	auto imageA = m_imageA.lock();
	auto imageB = m_imageB.lock();
	if(!imageA || !imageB)
	{
		PH_LOG_WARNING(BinaryMixedImage,
			"image pair is incomplete");
	}

	return {std::move(imageA), std::move(imageB)};
}

}// end namespace ph
