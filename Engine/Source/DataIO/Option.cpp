#include "DataIO/Option.h"

namespace ph
{

// command interface

Option::Option(const InputPacket& packet)
{}

SdlTypeInfo Option::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_OPTION, "option");
}

void Option::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
