#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/sdl_helpers.h"

namespace ph
{

const Logger SdlClass::logger(LogSender("SDL Class"));

std::string SdlClass::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

std::string SdlClass::genCategoryName() const
{
	return sdl::category_to_string(getCategory());
}

}// end namespace ph
