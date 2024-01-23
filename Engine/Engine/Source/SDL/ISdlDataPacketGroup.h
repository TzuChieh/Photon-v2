#pragma once

#include "Utility/utility.h"

#include <string_view>

namespace ph
{

class SdlInputClauses;

class ISdlDataPacketGroup
{
public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS_NO_DTOR(ISdlDataPacketGroup);
	virtual ~ISdlDataPacketGroup() = default;

	virtual const SdlInputClauses* get(std::string_view packetName) const = 0;
};

}// end namespace ph
