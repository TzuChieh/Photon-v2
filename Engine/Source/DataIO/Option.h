#pragma once

#include "DataIO/SDL/TCommandInterface.h"

namespace ph
{

class Option : public TCommandInterface<Option>
{
public:
	inline Option() = default;
	virtual inline ~Option() = default;

// command interface
public:
	explicit Option(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph
