#include "Actor/SDLExtension/color_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Tokenizer.h"
#include "Common/assertion.h"
#include "Actor/Image/ConstantImage.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/io_utils.h"
#include "Actor/Image/LdrPictureImage.h"
#include "Actor/Image/HdrPictureImage.h"

#include <vector>

namespace ph::sdl
{

math::Vector3R load_constant_values(const std::string& sdlValue)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlValue, tokens);

		// 3 input values form a vec3 exactly
		if(tokens.size() == 3)
		{
			return math::Vector3R(
				load_real(tokens[0]),
				load_real(tokens[1]),
				load_real(tokens[2]));
		}
		// 1 input value results in vec3 filled with the same value
		else if(tokens.size() == 1)
		{
			return math::Vector3R(load_real(tokens[0]));
		}
		else
		{
			throw SdlLoadError(
				"invalid constant representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing constant -> " + e.whatStr());
	}
}

std::shared_ptr<Image> load_constant_color(const math::Vector3R& values)
{
	return std::make_shared<ConstantImage>(
		values,
		ConstantImage::EType::RAW);
}

std::shared_ptr<Image> load_picture_color(const Path& filePath)
{
	const std::string extension = filePath.getExtension();

	// TODO: what if a format supports both LDR & HDR?

	if(io_utils::has_LDR_support(extension))
	{
		return std::make_shared<LdrPictureImage>(io_utils::load_LDR_picture(filePath));
	}
	else if(io_utils::has_HDR_support(extension))
	{
		return std::make_shared<HdrPictureImage>(io_utils::load_HDR_picture(filePath));
	}
	else
	{
		throw SdlLoadError(
			"on parsing picture color -> unsupported format: " + filePath.toAbsoluteString());
	}
}

}// end namespace ph::sdl
