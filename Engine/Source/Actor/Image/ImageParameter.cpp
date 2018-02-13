#include "Actor/Image/ImageParameter.h"
#include "FileIO/InputPacket.h"

namespace ph
{

ImageParameter::ImageParameter() : 
	m_sampleMode(EImgSampleMode::NEAREST),
	m_wrapMode(EImgWrapMode::REPEAT)
{}

ImageParameter& ImageParameter::setSampleMode(EImgSampleMode mode)
{
	m_sampleMode = mode;
}

ImageParameter& ImageParameter::setWrapMode(EImgWrapMode mode)
{
	m_wrapMode = mode;
}

EImgSampleMode ImageParameter::getSampleMode() const
{
	return m_sampleMode;
}

EImgWrapMode ImageParameter::getWrapMode() const
{
	return m_wrapMode;
}

// command interface

ImageParameter::ImageParameter(const InputPacket& packet) : 
	ImageParameter()
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

}// end namespace ph
