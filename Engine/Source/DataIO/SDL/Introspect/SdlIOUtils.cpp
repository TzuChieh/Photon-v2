#include "DataIO/SDL/Introspect/SdlIOUtils.h"
#include "DataIO/SDL/Tokenizer.h"

#include <string>
#include <exception>

namespace ph
{

std::optional<real> SdlIOUtils::loadReal(
	const std::string& sdlReal, std::string* const out_loaderMsg)
{
	// TODO: check for overflow?

	try
	{
		return static_cast<real>(std::stold(sdlReal));
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
	const std::string& sdlInteger, std::string* const out_loaderMsg)
{
	// TODO: check for overflow?

	try
	{
		return static_cast<integer>(std::stoll(sdlInteger));
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
	const std::string& sdlVector3R, std::string* const out_loaderMsg)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlVector3R, tokens);

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
	const std::string& sdlQuaternionR, std::string* const out_loaderMsg)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlQuaternionR, tokens);

		if(tokens.size() != 3)
		{
			if(out_loaderMsg)
			{
				*out_loaderMsg += "bad QuaternionR representation <" + sdlQuaternionR + ">";
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

}// end namespace ph
