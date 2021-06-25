#include "Actor/Dome/AImageDome.h"
#include "DataIO/PictureLoader.h"
#include "Actor/Image/HdrPictureImage.h"
#include "Common/Logger.h"

namespace ph
{

namespace
{

const Logger logger(LogSender("Actor Dome (Image)"));

}

AImageDome::AImageDome() :
	AImageDome(Path())
{}

AImageDome::AImageDome(const Path& imagePath) :

	ADome(),

	m_imagePath      (imagePath),
	m_imageResolution(1, 1)
{}

AImageDome::AImageDome(const AImageDome& other) :

	ADome(other),

	m_imagePath      (other.m_imagePath),
	m_imageResolution(other.m_imageResolution)
{}

std::shared_ptr<TTexture<Spectrum>> AImageDome::loadRadianceFunction(CookingContext& context)
{
	auto frame = PictureLoader::loadHdr(m_imagePath);

	// Since we are viewing it from inside a sphere
	frame.flipHorizontally();

	m_imageResolution = math::Vector2S(frame.getSizePx());

	auto image = std::make_shared<HdrPictureImage>(std::move(frame));
	image->setSampleMode(EImgSampleMode::BILINEAR);
	image->setWrapMode(EImgWrapMode::REPEAT);

	return image->genTextureSpectral(context);
}

math::Vector2S AImageDome::getResolution() const
{
	return m_imageResolution;
}

AImageDome& AImageDome::operator = (AImageDome rhs)
{
	swap(*this, rhs);

	return *this;
}

void swap(AImageDome& first, AImageDome& second)
{
	// Enable ADL
	using std::swap;

	swap(static_cast<ADome&>(first), static_cast<ADome&>(second));
	swap(first.m_imagePath,          second.m_imagePath);
	swap(first.m_imageResolution,    second.m_imageResolution);
}

}// end namespace ph
