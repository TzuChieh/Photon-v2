#pragma once

namespace ph
{

enum class EImgSampleMode
{
	NEAREST,
	MIPMAP_TRILINEAR
};

enum class EImgWrapMode
{
	REPEAT
};

class InputPacket;

class ImageParameter final
{
public:
	ImageParameter();

	ImageParameter& setSampleMode(EImgSampleMode mode);
	ImageParameter& setWrapMode(EImgWrapMode mode);

	EImgSampleMode getSampleMode() const;
	EImgWrapMode   getWrapMode() const;

private:
	EImgSampleMode m_sampleMode;
	EImgWrapMode   m_wrapMode;

// command interface
public:
	ImageParameter(const InputPacket& packet);
};

}// end namespace ph