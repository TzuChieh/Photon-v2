#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <type_traits>
#include <string>
#include <utility>

namespace ph
{

/*! @brief A field class that binds a integral member variable.
*/
template<typename Owner, typename IntType = integer, typename SdlValueType = TSdlValue<IntType, Owner>>
class TSdlInteger : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<IntType, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

public:
	template<typename ValueType>
	inline TSdlInteger(std::string valueName, ValueType Owner::* const valuePtr) : 
		SdlValueType("integer", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const IntType& value) const override
	{
		return std::to_string(value);
	}

	inline ESdlDataFormat getNativeFormat() const override
	{
		return ESdlDataFormat::Single;
	}

	inline ESdlDataType getNativeType() const override
	{
		return sdl::int_type_of<IntType>();
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_int<IntType>(payload.value));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		if(const IntType* const value = this->getValue(owner); value)
		{
			sdl::save_field_id(this, out_payload);
			sdl::save_int<IntType>(*value, &out_payload.value);
		}
	}
};

/*! @brief A field class that binds an optional integral member variable.
*/
template<typename Owner, typename IntType = integer>
using TSdlOptionalInteger = TSdlInteger<Owner, IntType, TSdlOptionalValue<IntType, Owner>>;

template<typename Owner>
using TSdlInt8 = TSdlInteger<Owner, int8>;

template<typename Owner>
using TSdlUint8 = TSdlInteger<Owner, uint8>;

template<typename Owner>
using TSdlInt16 = TSdlInteger<Owner, int16>;

template<typename Owner>
using TSdlUint16 = TSdlInteger<Owner, uint16>;

template<typename Owner>
using TSdlInt32 = TSdlInteger<Owner, int32>;

template<typename Owner>
using TSdlUint32 = TSdlInteger<Owner, uint32>;

template<typename Owner>
using TSdlInt64 = TSdlInteger<Owner, int64>;

template<typename Owner>
using TSdlUint64 = TSdlInteger<Owner, uint64>;

template<typename Owner>
using TSdlSize = TSdlInteger<Owner, std::size_t>;

template<typename Owner>
using TSdlOptionalInt8 = TSdlOptionalInteger<Owner, int8>;

template<typename Owner>
using TSdlOptionalUint8 = TSdlOptionalInteger<Owner, uint8>;

template<typename Owner>
using TSdlOptionalInt16 = TSdlOptionalInteger<Owner, int16>;

template<typename Owner>
using TSdlOptionalUint16 = TSdlOptionalInteger<Owner, uint16>;

template<typename Owner>
using TSdlOptionalInt32 = TSdlOptionalInteger<Owner, int32>;

template<typename Owner>
using TSdlOptionalUint32 = TSdlOptionalInteger<Owner, uint32>;

template<typename Owner>
using TSdlOptionalInt64 = TSdlOptionalInteger<Owner, int64>;

template<typename Owner>
using TSdlOptionalUint64 = TSdlOptionalInteger<Owner, uint64>;

template<typename Owner>
using TSdlOptionalSize = TSdlOptionalInteger<Owner, std::size_t>;

}// end namespace ph
