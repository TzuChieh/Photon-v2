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

/*! @brief A field class that binds a real member variable.
*/
template<typename Owner, typename RealType = real, typename SdlValueType = TSdlValue<RealType, Owner>>
class TSdlReal : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<RealType, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

	static_assert(std::is_same_v<RealType, real>, 
		"Currently supports only ph::real");

public:
	template<typename ValueType>
	inline TSdlReal(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("real", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const RealType& value) const override
	{
		return std::to_string(value);
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override
	{
		setValue(owner, sdl::load_real(sdlValue));
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

/*! @brief A field class that binds an optional real member variable.
*/
template<typename Owner, typename RealType = real>
using TSdlOptionalReal = TSdlReal<Owner, RealType, TSdlOptionalValue<RealType, Owner>>;

}// end namespace ph
