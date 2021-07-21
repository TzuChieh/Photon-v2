#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "DataIO/SDL/sdl_helpers.h"

namespace ph
{

const Logger SdlFunction::logger(LogSender("SDL Function"));

std::string SdlFunction::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

}// end namespace ph