#include "Actor/Image/BinaryMixedImage.h"
#include "DataIO/SDL/InputPacket.h"
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

BinaryMixedImage::BinaryMixedImage(const InputPacket& packet) :
	Image(packet)
{
	m_imageA = packet.getReference<Image>("a", DataTreatment::REQUIRED());
	m_imageB = packet.getReference<Image>("b", DataTreatment::REQUIRED());
}

SdlTypeInfo BinaryMixedImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "binary");
}

}// end namespace ph
