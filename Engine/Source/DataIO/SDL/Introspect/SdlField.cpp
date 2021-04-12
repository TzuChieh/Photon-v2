#include "DataIO/SDL/Introspect/SdlField.h"
#include "DataIO/SDL/SdlIOUtils.h"

namespace ph
{

const Logger SdlField::logger(LogSender("SDL Field"));

std::string SdlField::genPrettyName() const
{
	return SdlIOUtils::genPrettyName(this);
}

}// end namespace ph