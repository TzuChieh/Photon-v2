#include "DataIO/SDL/Introspect/SDLIOContext.h"
#include "DataIO/SDL/Introspect/SdlClass.h"

namespace ph
{

std::string SDLIOContext::genPrettySrcClassName() const
{
	return getSrcClass() ? getSrcClass()->genPrettyName() : "unavailable";
}

}// end namespace ph
