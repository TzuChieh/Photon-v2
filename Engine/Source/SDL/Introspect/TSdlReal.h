#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"

#include <type_traits>
#include <string>
#include <utility>

namespace ph
{

/*! @brief A field class that binds a floating point member variable.
*/
template<typename Owner, typename FloatType = real, typename SdlValueType = TSdlValue<FloatType, Owner>>
class TSdlReal : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<FloatType, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlReal(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("real", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const FloatType& value) const override
	{
		return std::to_string(value);
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		FloatType* const realPtr = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<FloatType, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optReal = this->valueRef(owner)](std::size_t /* elementIdx */) -> SdlGetterVariant
				{
					return optReal
						? SdlNativeData::permissiveElementGetter(&(*optReal))
						: std::monostate{};
				},
				[&optReal = this->valueRef(owner)](std::size_t /* elementIdx */, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optReal = std::nullopt;
						return true;
					}
					else
					{
						optReal = FloatType{};
						return SdlNativeData::permissiveElementSetter(input, &(*optReal));
					}
				},
				AnyNonConstPtr(realPtr));

			data.numElements = realPtr ? 1 : 0;
			data.elementContainer = ESdlDataFormat::Single;
			data.elementType = sdl::float_type_of<FloatType>();
			data.isNullClearable = true;
			return data;
		}
		else
		{
			return SdlNativeData::fromSingleElement(
				realPtr, ESdlDataFormat::Single, sdl::float_type_of<FloatType>(), true, true);
		}
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_float<FloatType>(clause.value));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const FloatType* const value = this->getConstValue(owner); value)
		{
			sdl::save_float<FloatType>(*value, &out_clause.value);
		}
	}
};

/*! @brief A field class that binds an optional floating point member variable.
*/
template<typename Owner, typename FloatType = real>
using TSdlOptionalReal = TSdlReal<Owner, FloatType, TSdlOptionalValue<FloatType, Owner>>;

template<typename Owner>
using TSdlFloat = TSdlReal<Owner, float>;

template<typename Owner>
using TSdlDouble = TSdlReal<Owner, double>;

template<typename Owner>
using TSdlFloat32 = TSdlReal<Owner, float32>;

template<typename Owner>
using TSdlFloat64 = TSdlReal<Owner, float64>;

template<typename Owner>
using TSdlOptionalFloat = TSdlOptionalReal<Owner, float>;

template<typename Owner>
using TSdlOptionalDouble = TSdlOptionalReal<Owner, double>;

template<typename Owner>
using TSdlOptionalFloat32 = TSdlOptionalReal<Owner, float32>;

template<typename Owner>
using TSdlOptionalFloat64 = TSdlOptionalReal<Owner, float64>;

}// end namespace ph
