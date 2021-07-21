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

/*! @brief A field class that binds a floating point member variable.
*/
template<typename Owner, typename FloatType = real, typename SdlValueType = TSdlValue<FloatType, Owner>>
class TSdlReal : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<FloatType, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

public:
	template<typename ValueType>
	inline TSdlReal(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("real", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const FloatType& value) const override
	{
		return std::to_string(value);
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlPayload&      payload,
		const SdlInputContext& ctx) const override
	{
		setValue(owner, sdl::load_float<FloatType>(payload.value));
	}

	inline void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const
	{
		PH_ASSERT(out_sdlValue);

		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
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
