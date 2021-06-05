#pragma once

#include "DataIO/SDL/TCommandInterface.h"

namespace ph
{

class Option : public TCommandInterface<Option>
{
public:
	inline Option() = default;
	virtual inline ~Option() = default;

	ETypeCategory getCategory() const override;

// command interface
public:
	explicit Option(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline ETypeCategory Option::getCategory() const
{
	return ETypeCategory::REF_OPTION;
}

}// end namespace ph
