#include "Core/Texture/TextureLoader.h"
#include "Core/Texture/LdrRgbTexture2D.h"
#include "Math/Random.h"
#include "Core/Quantity/ColorSpace.h"

#include "Common/ThirdParty/lib_stb.h"

#include <iostream>

namespace ph
{

TextureLoader::TextureLoader()
{

}

bool TextureLoader::load(const std::string& fullFilename, RgbPixelTexture* const out_rgbPixelTexture)
{
	//std::cout << "loading image <" << fullFilename << ">" << std::endl;

	//// variables to retrieve image info from stbi_load()
	//int widthPx;
	//int heightPx;
	//int numComponents;

	//// default loading's origin is on the upper-left corner, this call made stb made the 
	//// origin on the lower-left corner to meet with Photon's expectation
	//stbi_set_flip_vertically_on_load(true);

	//// the last parameter is "0" since we want the actual components the image has;
	//// replace "0" with "1" ~ "4" to force that many components per pixel
	//stbi_uc* stbImageData = stbi_load(fullFilename.c_str(), &widthPx, &heightPx, &numComponents, 0);

	//if(stbImageData == NULL)
	//{
	//	std::cerr << "warning: at TextureLoader::load(), file <" << fullFilename << "> loading failed" << std::endl;
	//	std::cerr << "(message: " << stbi_failure_reason() << ")" << std::endl;
	//	return false;
	//}

	//if(numComponents != 3)
	//{
	//	std::cerr << "warning: at TextureLoader::load(), " 
	//	          << "texture <" << fullFilename << "> #comp. != 3, unsupported" << std::endl;
	//	return false;
	//}

	//const uint32 dataSize = widthPx * heightPx * numComponents;
	//std::vector<real> pixelData(static_cast<std::size_t>(dataSize), 0.0_r);
	//for(uint32 i = 0; i < dataSize; i += 3)
	//{
	//	const Vector3R sRgbPixel(stbImageData[i + 0] / 255.0_r, 
	//	                         stbImageData[i + 1] / 255.0_r, 
	//	                         stbImageData[i + 2] / 255.0_r);

	//	const Vector3R linearRgb = ColorSpace::sRgbToLinearRgb(sRgbPixel);

	//	pixelData[i + 0] = linearRgb.x;
	//	pixelData[i + 1] = linearRgb.y;
	//	pixelData[i + 2] = linearRgb.z;
	//}

	//// free the image data loaded by stb
	//stbi_image_free(stbImageData);

	//out_rgbPixelTexture->resize(static_cast<uint32>(widthPx), static_cast<uint32>(heightPx));
	//out_rgbPixelTexture->setPixels(0, 0, 
	//                               static_cast<uint32>(widthPx), static_cast<uint32>(heightPx), 
	//                               pixelData.data());

	//return true;

	return false;
}

}// end namespace ph
