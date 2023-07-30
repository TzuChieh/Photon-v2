#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"
#include "SDL/Introspect/TSdlEnum.h"

#include <type_traits>
#include <string>
#include <utility>

namespace ph
{

/*
@note The definition for SDL enum, i.e., the definition body of `PH_DEFINE_SDL_ENUM()` must
present before the use of the enum field.
*/
template<typename Owner, typename EnumType, typename SdlValueType = TSdlValue<EnumType, Owner>>
class TSdlEnumField : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<EnumType, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

	static_assert(std::is_enum_v<EnumType>,
		"EnumType must be a C++ enum.");

public:
	template<typename ValueType>
	inline explicit TSdlEnumField(ValueType Owner::* const valuePtr) :
		TSdlEnumField(TSdlEnum<EnumType>::getSdlEnum()->getName(), valuePtr)
	{}

	template<typename ValueType>
	inline TSdlEnumField(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("enum", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const EnumType& value) const override
	{
		return std::string(TSdlEnum<EnumType>()[value]);
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		EnumType* const enumPtr = this->getValue(owner);

		SdlNativeData data;
		if(enumPtr)
		{
			data = SdlNativeData::fromSingleElement(enumPtr, true, true);
		}
		data.elementContainer = ESdlDataFormat::Single;
		data.elementType = ESdlDataType::Enum;
		return data;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, TSdlEnum<EnumType>()[clause.value]);
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const EnumType* const enumValue = this->getConstValue(owner); enumValue)
		{
			out_clause.value = TSdlEnum<EnumType>()[*enumValue];
		}
	}
};

template<typename Owner, typename EnumType>
using TSdlOptionalEnumField = TSdlEnumField<Owner, EnumType, TSdlOptionalValue<EnumType, Owner>>;

}// end namespace ph
