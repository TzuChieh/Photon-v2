#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/SdlIOUtils.h"

namespace ph
{

const Logger SdlClass::logger(LogSender("SDL Class"));

std::string SdlClass::genPrettyName() const
{
	return SdlIOUtils::genPrettyName(this);
}

}// end namespace ph
