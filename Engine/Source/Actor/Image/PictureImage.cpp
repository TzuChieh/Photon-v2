#include "Actor/Image/PictureImage.h"
#include "FileIO/InputPacket.h"

namespace ph
{

PictureImage::PictureImage() :
	Image(),
	m_sampleMode(EImgSampleMode::NEAREST),
	m_wrapMode(EImgWrapMode::REPEAT)
{}

PictureImage::~PictureImage() = default;

PictureImage& PictureImage::setSampleMode(EImgSampleMode mode)
{
	m_sampleMode = mode;
}

PictureImage& PictureImage::setWrapMode(EImgWrapMode mode)
{
	m_wrapMode = mode;
}

EImgSampleMode PictureImage::getSampleMode() const
{
	return m_sampleMode;
}

EImgWrapMode PictureImage::getWrapMode() const
{
	return m_wrapMode;
}

// command interface

PictureImage::PictureImage(const InputPacket& packet) : 
	Image(packet)
{
	const std::string& sampleMode = packet.getString("sample-mode", "nearest");
	if(sampleMode == "nearest")
	{
		m_sampleMode = EImgSampleMode::NEAREST;
	}
	else if(sampleMode == "mipmap-trilinear")
	{
		m_sampleMode = EImgSampleMode::MIPMAP_TRILINEAR;
	}

	const std::string& wrapMode = packet.getString("wrap-mode", "repeat");
	if(wrapMode == "repeat")
	{
		m_wrapMode = EImgWrapMode::REPEAT;
	}
}

SdlTypeInfo PictureImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "picture");
}

void PictureImage::ciRegister(CommandRegister& cmdRegister)
{

}

}// end namespace ph