#pragma once

#include <string>

namespace ph
{

class PixelTexture;

class TextureLoader final
{
public:
	TextureLoader();

	bool load(const std::string& fullFilename, PixelTexture* const out_pixelTexture);
};

}// end namespace ph