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
		const SdlPayload&      payload,
		const SdlInputContext& ctx) const override
	{
		// Save <sdlValue> directly as it is already a string
		setValue(owner, std::string(payload.value));
	}

	inline void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override
	{
		PH_ASSERT(out_sdlValue);

		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
};

template<typename Owner>
using TSdlOptionalString = TSdlString<Owner, TSdlOptionalValue<std::string, Owner>>;

}// end namespace ph
