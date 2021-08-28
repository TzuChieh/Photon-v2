#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <string>

namespace ph
{

template<typename Owner, typename SdlValueType = TSdlValue<std::string, Owner>>
class TSdlString : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<std::string, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

public:
	template<typename ValueType>
	inline TSdlString(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("string", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const std::string& str) const override
	{
		return str;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		// Load <sdlValue> directly as it is already a string
		this->setValue(owner, std::string(payload.value));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		if(const std::string* const str = this->getValue(owner); str)
		{
			// Save <sdlValue> directly as it is already a string
			out_payload.value = *str;
		}
	}
};

template<typename Owner>
using TSdlOptionalString = TSdlString<Owner, TSdlOptionalValue<std::string, Owner>>;

}// end namespace ph
