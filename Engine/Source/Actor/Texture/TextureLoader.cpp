#include "Actor/Texture/TextureLoader.h"
#include "Actor/Texture/PixelTexture.h"
#include "Math/random_number.h"

#include "Common/ThirdParty/lib_stb.h"

#include <iostream>

namespace ph
{

TextureLoader::TextureLoader()
{

}

bool TextureLoader::load(const std::string& fullFilename, PixelTexture* const out_pixelTexture)
{
	std::cout << "loading image <" << fullFilename << ">" << std::endl;

	// variables to retrieve image info from stbi_load()
	int widthPx;
	int heightPx;
	int numComponents;

	// default loading's origin is on the upper-left corner, this call made stb made the 
	// origin on the lower-left corner to meet with Photon's expectation
	stbi_set_flip_vertically_on_load(true);

	// the last parameter is "0" since we want the actual components the image has;
	// replace "0" with "1" ~ "4" to force that many components per pixel
	stbi_uc* stbImageData = stbi_load(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	if(stbImageData == NULL)
	{
		std::cerr << "warning: at TextureLoader::load(), file <" << fullFilename << "> loading failed" << std::endl;
		std::cerr << "(message: " << stbi_failure_reason() << ")" << std::endl;
		return false;
	}

	//std::cout << "# " << numComponents << std::endl;

	const uint32 dataSize = widthPx * heightPx * numComponents;
	std::vector<real> pixelData(static_cast<std::size_t>(dataSize), 0.0_r);
	for(uint32 i = 0; i < dataSize; i++)
	{
		pixelData[i] = static_cast<real>(stbImageData[i]) / 255.0_r;
	}

	// don't forget to free the image data loaded by stb
	stbi_image_free(stbImageData);

	out_pixelTexture->reset(static_cast<uint32>(widthPx), static_cast<uint32>(heightPx), static_cast<uint32>(numComponents));
	out_pixelTexture->setPixels(0, 0, static_cast<uint32>(widthPx), static_cast<uint32>(heightPx), static_cast<uint32>(numComponents), pixelData.data());

	return true;
}

}// end namespace ph