#include "DataIO/SDL/Introspect/SdlField.h"
#include "DataIO/SDL/sdl_helpers.h"

namespace ph
{

const Logger SdlField::logger(LogSender("SDL Field"));

std::string SdlField::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

}// end namespace ph