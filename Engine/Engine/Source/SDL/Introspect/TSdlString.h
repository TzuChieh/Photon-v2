#pragma once

#include "SDL/Introspect/TSdlValue.h"

#include <Common/assertion.h>

#include <string>

namespace ph
{

template<typename Owner, typename SdlValueType = TSdlValue<std::string, Owner>>
class TSdlString : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<std::string, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	TSdlString(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("string", std::move(valueName), valuePtr)
	{}

	std::string valueAsString(const std::string& str) const override
	{
		return str;
	}

	SdlNativeData ownedNativeData(Owner& owner) const override
	{
		std::string* const str = this->getValue(owner);
		return SdlNativeData::fromSingleElement(
			str, ESdlDataFormat::Single, ESdlDataType::String, true, true);
	}

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		// Load value directly as it is already a string
		this->setValue(owner, clause.value);
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		// Not testing whether the string contains whitespaces--just add double quotes
		out_clause.value = '"';

		if(const std::string* str = this->getConstValue(owner); str)
		{
			// Append value directly as it is already a string
			out_clause.value += *str;
		}

		out_clause.value += '"';
	}
};

}// end namespace ph
