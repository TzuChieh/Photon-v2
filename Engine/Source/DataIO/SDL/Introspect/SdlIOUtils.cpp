#include "DataIO/SDL/Introspect/SdlIOUtils.h"
#include "DataIO/SDL/Tokenizer.h"

#include <string>
#include <exception>

namespace ph
{

std::optional<real> SdlIOUtils::loadReal(
	const std::string& sdlRealStr, std::string* const out_loaderMsg)
{
	// TODO: check for overflow?

	try
	{
		return static_cast<real>(std::stold(sdlRealStr));
	}
	catch(const std::exception& e)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "exception on parsing real (" + std::string(e.what()) + ")";
		}

		return std::nullopt;
	}
	catch(...)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "unknown exception occurred on parsing real";
		}

		return std::nullopt;
	}
}

std::optional<integer> SdlIOUtils::loadInteger(
	const std::string& sdlIntegerStr, std::string* const out_loaderMsg)
{
	// TODO: check for overflow?

	try
	{
		return static_cast<integer>(std::stoll(sdlIntegerStr));
	}
	catch(const std::exception& e)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "exception on parsing integer (" + std::string(e.what()) + ")";
		}

		return std::nullopt;
	}
	catch(...)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "unknown exception occurred on parsing integer";
		}

		return std::nullopt;
	}
}

std::optional<math::Vector3R> SdlIOUtils::loadVector3R(
	const std::string& sdlVector3Str, std::string* const out_loaderMsg)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlVector3Str, tokens);

		if(tokens.size() != 3)
		{
			if(out_loaderMsg)
			{
				*out_loaderMsg += "bad Vector3R representation <" + sdlVector3R + ">";
			}

			return std::nullopt;
		}

		return math::Vector3R(
			static_cast<real>(std::stold(tokens[0])),
			static_cast<real>(std::stold(tokens[1])),
			static_cast<real>(std::stold(tokens[2])));
	}
	catch(const std::exception& e)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "exception on parsing Vector3R (" + std::string(e.what()) + ")";
		}

		return std::nullopt;
	}
	catch(...)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "unknown exception occurred on parsing Vector3R";
		}

		return std::nullopt;
	}
}

std::optional<math::QuaternionR> SdlIOUtils::loadQuaternionR(
	const std::string& sdlQuaternionStr, std::string* const out_loaderMsg)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlQuaternionStr, tokens);

		if(tokens.size() != 3)
		{
			if(out_loaderMsg)
			{
				*out_loaderMsg += "bad QuaternionR representation <" + sdlQuaternionStr + ">";
			}

			return std::nullopt;
		}

		return math::QuaternionR(
			static_cast<real>(std::stold(tokens[0])),
			static_cast<real>(std::stold(tokens[1])),
			static_cast<real>(std::stold(tokens[2])), 
			static_cast<real>(std::stold(tokens[3])));
	}
	catch(const std::exception& e)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "exception on parsing QuaternionR (" + std::string(e.what()) + ")";
		}

		return std::nullopt;
	}
	catch(...)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "unknown exception occurred on parsing QuaternionR";
		}

		return std::nullopt;
	}
}

std::optional<std::vector<real>> SdlIOUtils::loadRealArray(
	const std::string& sdlRealArrayStr, std::string* const out_loaderMsg)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		// Tries to tokenize and see if the tokens are valid array or in fact
		// an identifier. If it is an identifier, load the actual tokens.
		std::vector<std::string> realTokens;
		tokenizer.tokenize(sdlRealArrayStr, realTokens);
		if(!realTokens.empty())
		{
			if(!startsWithNumber(realTokens[0]))
			{
				const std::string& identifier = sdlRealArrayStr;
				realTokens.clear();
				tokenizer.tokenize(loadResource(identifier), realTokens);
			}
		}

		std::vector<real> realArray;
		for(const auto& realToken : realTokens)
		{
			const auto realValue = std::stold(realToken);
			realArray.push_back(static_cast<real>(realValue));
		}

		return std::move(realArray);
	}
	catch(const std::exception& e)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "exception on parsing real array (" + std::string(e.what()) + ")";
		}

		return std::nullopt;
	}
	catch(...)
	{
		if(out_loaderMsg)
		{
			*out_loaderMsg += "unknown exception occurred on parsing real array";
		}

		return std::nullopt;
	}
}

}// end namespace ph
