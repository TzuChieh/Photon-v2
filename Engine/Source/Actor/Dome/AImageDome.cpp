#include "Actor/Dome/AImageDome.h"
#include "Actor/Image/RasterFileImage.h"

namespace ph
{

AImageDome::AImageDome() :
	AImageDome(Path())
{}

AImageDome::AImageDome(const Path& imagePath) :

	ADome(),

	m_imagePath      (imagePath),
	m_imageResolution(1, 1)
{}

std::shared_ptr<TTexture<math::Spectrum>> AImageDome::loadRadianceFunction(ActorCookingContext& ctx)
{
	RasterFileImage image(m_imagePath);
	image.setSampleMode(EImageSampleMode::Bilinear);

	// Since we are viewing it from inside a sphere, we flip the image horizontally
	image.setWrapMode(EImageWrapMode::FlippedClampToEdge, EImageWrapMode::ClampToEdge);

	auto radianceFunc = image.genColorTexture(ctx);

	// Access image property after cooking for an up-to-date value
	m_imageResolution = math::Vector2S(image.getResolution());

	return radianceFunc;
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
