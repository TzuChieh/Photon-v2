#include "DataIO/SDL/Introspect/SdlIOContext.h"
#include "DataIO/SDL/Introspect/SdlClass.h"

namespace ph
{

std::string SdlIOContext::genPrettySrcClassName() const
{
	return getSrcClass() ? getSrcClass()->genPrettyName() : "unavailable";
}

}// end namespace ph
