#include "SDL/Introspect/SdlFunction.h"
#include "SDL/sdl_helpers.h"

namespace ph
{

std::string SdlFunction::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

}// end namespace ph
