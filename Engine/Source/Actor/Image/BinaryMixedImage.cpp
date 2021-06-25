#include "Actor/Image/BinaryMixedImage.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{

Logger logger(LogSender("Binary Mixed Image"));

}

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
		logger.log(ELogLevel::WARNING_MED,
			"image pair is incomplete");
	}

	return {std::move(imageA), std::move(imageB)};
}

}// end namespace ph
