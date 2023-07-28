#include "SDL/Introspect/SdlField.h"
#include "SDL/sdl_helpers.h"

namespace ph
{

SdlField::~SdlField() = default;

std::string SdlField::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

}// end namespace ph