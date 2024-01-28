#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "SDL/sdl_helpers.h"

#include <Common/config.h>
#include <Common/logging.h>

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

			data.numElements = boolPtr ? 1 : 0;
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
		if(clause.value == "true" || clause.value == "TRUE" || clause.value == "True")
		{
			this->setValue(owner, true);
		}
#if !PH_DEBUG
		else
		{
			this->setValue(owner, false);
		}
#else
		else if(clause.value == "false" || clause.value == "FALSE" || clause.value == "False")
		{
			this->setValue(owner, false);
		}
		else
		{
			PH_DEFAULT_DEBUG_LOG(
				"Unrecognized SDL bool literal: {}, treating it as false.", clause.value);

			this->setValue(owner, false);
		}
#endif
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const bool* value = this->getConstValue(owner); value)
		{
			out_clause.value = *value ? "true" : "false";
		}
		else
		{
			out_clause.isEmpty = true;
		}
	}
};

}// end namespace ph
