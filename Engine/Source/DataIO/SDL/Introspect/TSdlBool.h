#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/sdl_helpers.h"

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
			data = SdlNativeData(boolPtr);
		}

		data.format = ESdlDataFormat::Single;
		data.dataType = ESdlDataType::Bool;

		return data;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		if(payload.value == "true" || payload.value == "TRUE")
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
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		if(const bool* const value = this->getConstValue(owner); value)
		{
			sdl::save_field_id(this, out_payload);
			out_payload.value = *value ? "true" : "false";
		}
	}
};

}// end namespace ph
