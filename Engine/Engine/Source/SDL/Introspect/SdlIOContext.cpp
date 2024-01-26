#include "SDL/Introspect/SdlIOContext.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/sdl_helpers.h"

namespace ph
{

std::string SdlIOContext::genPrettySrcInfo() const
{
	std::string info;
	
	if(getSrcClass())
	{
		if(!info.empty())
		{
			info += ", ";
		}

		info += "class <";
		info += sdl::gen_pretty_name(getSrcClass());
		info += ">";
	}

	if(getSrcStruct())
	{
		if(!info.empty())
		{
			info += ", ";
		}

		info += "struct <";
		info += sdl::gen_pretty_name(getSrcStruct());
		info += ">";
	}

	return info.empty() ? "unavailable" : info;
}

}// end namespace ph
