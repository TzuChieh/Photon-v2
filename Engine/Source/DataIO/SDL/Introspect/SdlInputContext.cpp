#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/Introspect/SdlClass.h"

namespace ph
{

std::string SdlInputContext::genPrettySrcClassName() const
{
	return srcClass ? srcClass->genPrettyName() : "unavailable";
}

}// end namespace ph
