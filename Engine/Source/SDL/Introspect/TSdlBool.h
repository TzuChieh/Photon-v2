#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/sdl_helpers.h"

#include <type_traits>
#include <string>
#include <utility>

namespace ph
{

/*! @brief A field class that binds a bool member variable.
*/
template<typename Owner, typename SdlValueType = TSdlValue<bool, Owner>>
class TSdlBool : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<bool, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlBool(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("bool", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const bool& value) const override
	{
		return value ? "true" : "false";
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		bool* const boolPtr = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<bool, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optBool = this->valueRef(owner)](std::size_t /* elementIdx */) -> SdlGetterVariant
				{
					return optBool
						? SdlNativeData::permissiveElementGetter(&(*optBool))
						: std::monostate{};
				},
				[&optBool = this->valueRef(owner)](std::size_t /* elementIdx */, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optBool = std::nullopt;
						return true;
					}
					else
					{
						optBool = bool{};
						return SdlNativeData::permissiveElementSetter(input, &(*optBool));
					}
				},
				AnyNonConstPtr(boolPtr));

			data.numElements = 1;
			data.elementContainer = ESdlDataFormat::Single;
			data.elementType = ESdlDataType::Bool;
			data.isNullClearable = true;
			return data;
		}
		else
		{
			return SdlNativeData::fromSingleElement(
				boolPtr, ESdlDataFormat::Single, ESdlDataType::Bool, true, true);
		}
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		if(clause.value == "true" || clause.value == "TRUE")
		{
			this->setValue(owner, true);
		}
		else
		{
			this->setValue(owner, false);
		}
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const bool* const value = this->getConstValue(owner); value)
		{
			out_clause.value = *value ? "true" : "false";
		}
	}
};

}// end namespace ph
