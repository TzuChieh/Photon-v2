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

		SdlNativeData data;
		if(boolPtr)
		{
			data = SdlNativeData::fromSingleElement(boolPtr, true, true);
		}
		data.elementContainer = ESdlDataFormat::Single;
		data.elementType = ESdlDataType::Bool;
		return data;
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
