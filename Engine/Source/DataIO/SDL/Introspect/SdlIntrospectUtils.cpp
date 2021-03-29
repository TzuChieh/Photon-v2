#include "DataIO/SDL/Introspect/SdlIntrospectUtils.h"

#include <string>
#include <exception>

namespace ph
{

std::optional<real> SdlIntrospectUtils::loadReal(const std::string& sdlValue, std::string* const out_loaderMsg)
{
	try
	{
		return std::stold(sdlValue);
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

std::optional<integer> SdlIntrospectUtils::loadInteger(const std::string& sdlValue, std::string* const out_loaderMsg)
{
	try
	{
		return std::stoll(sdlValue);
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

}// end namespace ph
