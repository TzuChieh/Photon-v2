#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Introspect/TSdlEnum.h"

#include <type_traits>
#include <string>
#include <utility>

namespace ph
{

template<typename Owner, typename EnumType, typename SdlValueType = TSdlValue<EnumType, Owner>>
class TSdlEnumField : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<EnumType, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

	static_assert(std::is_enum_v<EnumType>,
		"EnumType must be a C++ enum.");

public:
	template<typename ValueType>
	inline TSdlEnumField(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType(TSdlEnum<EnumType>::getSdlEnum()->getName(), std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const EnumType& value) const override
	{
		return TSdlEnum<EnumType>()[];
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlPayload&      payload,
		const SdlInputContext& ctx) const override
	{
		setValue(owner, sdl::load_integer(payload.value));
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

template<typename Owner, typename IntType = integer>
using TSdlOptionalInteger = TSdlInteger<Owner, IntType, TSdlOptionalValue<IntType, Owner>>;

}// end namespace ph
