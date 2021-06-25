#include "Actor/Image/PictureImage.h"

namespace ph
{

PictureImage::PictureImage() :
	Image(),
	m_sampleMode(EImgSampleMode::NEAREST),
	m_wrapMode(EImgWrapMode::REPEAT)
{}

PictureImage& PictureImage::setSampleMode(EImgSampleMode mode)
{
	m_sampleMode = mode;

	return *this;
}

PictureImage& PictureImage::setWrapMode(EImgWrapMode mode)
{
	m_wrapMode = mode;

	return *this;
}

EImgSampleMode PictureImage::getSampleMode() const
{
	return m_sampleMode;
}

EImgWrapMode PictureImage::getWrapMode() const
{
	return m_wrapMode;
}

}// end namespace ph
