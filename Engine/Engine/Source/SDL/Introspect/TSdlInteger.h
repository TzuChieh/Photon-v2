#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "SDL/sdl_helpers.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

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
	static_assert(std::is_base_of_v<TSdlAbstractValue<IntType, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlInteger(std::string valueName, ValueType Owner::* const valuePtr) : 
		SdlValueType("integer", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const IntType& value) const override
	{
		return std::to_string(value);
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		IntType* const intPtr = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<IntType, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optInt = this->valueRef(owner)](std::size_t /* elementIdx */) -> SdlGetterVariant
				{
					return optInt
						? SdlNativeData::permissiveElementGetter(&(*optInt))
						: std::monostate{};
				},
				[&optInt = this->valueRef(owner)](std::size_t /* elementIdx */, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optInt = std::nullopt;
						return true;
					}
					else
					{
						optInt = IntType{};
						return SdlNativeData::permissiveElementSetter(input, &(*optInt));
					}
				},
				AnyNonConstPtr(intPtr));

			data.numElements = intPtr ? 1 : 0;
			data.elementContainer = ESdlDataFormat::Single;
			data.elementType = sdl::int_type_of<IntType>();
			data.isNullClearable = true;
			return data;
		}
		else
		{
			return SdlNativeData::fromSingleElement(
				intPtr, ESdlDataFormat::Single, sdl::int_type_of<IntType>(), true, true);
		}
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_int<IntType>(clause.value));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const IntType* value = this->getConstValue(owner); value)
		{
			sdl::save_int<IntType>(*value, &out_clause.value);
		}
		else
		{
			out_clause.isEmpty = true;
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
using TSdlUInt32 = TSdlInteger<Owner, uint32>;

template<typename Owner>
using TSdlInt64 = TSdlInteger<Owner, int64>;

template<typename Owner>
using TSdlUInt64 = TSdlInteger<Owner, uint64>;

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
