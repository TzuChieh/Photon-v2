#pragma once

#include <string>

namespace ph
{

class RgbPixelTexture;

class TextureLoader final
{
public:
	TextureLoader();

	bool load(const std::string& fullFilename, RgbPixelTexture* const out_rgbPixelTexture);
};

}// end namespace ph