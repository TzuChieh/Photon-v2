#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "SDL/sdl_helpers.h"
#include "SDL/Introspect/TSdlEnum.h"

#include <Common/assertion.h>

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
	explicit TSdlEnumField(ValueType Owner::* const valuePtr)
		: TSdlEnumField(
			TSdlEnum<EnumType>::getSdlEnum()->getName(), 
			valuePtr)
	{}

	template<typename ValueType>
	TSdlEnumField(std::string valueName, ValueType Owner::* const valuePtr)
		: SdlValueType(
			"enum", 
			std::move(valueName), 
			"E/" + TSdlEnum<EnumType>::getSdlEnum()->getName(),
			valuePtr)
	{}

	std::string valueAsString(const EnumType& value) const override
	{
		return std::string(TSdlEnum<EnumType>()[value]);
	}

	SdlNativeData ownedNativeData(Owner& owner) const override
	{
		EnumType* const enumPtr = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<EnumType, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optEnum = this->valueRef(owner)](std::size_t /* elementIdx */) -> SdlGetterVariant
				{
					return optEnum
						? SdlNativeData::permissiveElementGetter(&(*optEnum))
						: std::monostate{};
				},
				[&optEnum = this->valueRef(owner)](std::size_t /* elementIdx */, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optEnum = std::nullopt;
						return true;
					}
					else
					{
						optEnum = EnumType{};
						return SdlNativeData::permissiveElementSetter(input, &(*optEnum));
					}
				},
				AnyNonConstPtr(enumPtr));

			data.numElements = enumPtr ? 1 : 0;
			data.elementContainer = ESdlDataFormat::Single;
			data.elementType = ESdlDataType::Enum;
			data.isNullClearable = true;
			return data;
		}
		else
		{
			return SdlNativeData::fromSingleElement(
				enumPtr, ESdlDataFormat::Single, ESdlDataType::Enum, true, true);
		}
	}

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, TSdlEnum<EnumType>()[clause.value]);
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const EnumType* enumValue = this->getConstValue(owner); enumValue)
		{
			out_clause.value = TSdlEnum<EnumType>()[*enumValue];
		}
		else
		{
			out_clause.isEmpty = true;
		}
	}
};

template<typename Owner, typename EnumType>
using TSdlOptionalEnumField = TSdlEnumField<Owner, EnumType, TSdlOptionalValue<EnumType, Owner>>;

}// end namespace ph
