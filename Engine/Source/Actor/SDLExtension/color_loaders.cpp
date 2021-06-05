#include "Actor/SDLExtension/color_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/Tokenizer.h"
#include "Common/assertion.h"
#include "Actor/Image/ConstantImage.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <vector>

namespace ph::sdl
{

std::shared_ptr<Image> load_constant_color(const std::string& sdlValue)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlValue, tokens);

		// Re-sample from multiple input values
		if(tokens.size() > 3)
		{
			// TODO: resample
			PH_ASSERT_UNREACHABLE_SECTION();
		}
		// Assume raw linear SRGB
		else if(tokens.size() == 3)
		{
			return std::make_shared<ConstantImage>(
				math::Vector3R(
					load_real(tokens[0]),
					load_real(tokens[1]),
					load_real(tokens[2])),
				ConstantImage::EType::RAW);
		}
		// Assume constant
		else if(tokens.size() == 1)
		{
			return std::make_shared<ConstantImage>(
				sdl::load_real(tokens[0]),
				ConstantImage::EType::RAW);
		}
		else
		{
			throw SdlLoadError(
				"invalid color representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing color -> " + e.whatStr());
	}
}

}// end namespace ph::sdl
