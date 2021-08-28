#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <type_traits>
#include <string>
#include <cstddef>

namespace ph
{

template<typename Owner, typename Element, typename SdlValueType = TSdlValue<math::TVector2<Element>, Owner>>
class TSdlVector2 : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<math::TVector2<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

public:
	template<typename ValueType>
	inline TSdlVector2(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("vector2", std::move(valueName), valuePtr)
	{
		if constexpr(std::is_same_v<SdlValueType, TSdlValue<math::TVector2<Element>, Owner>>)
		{
			this->defaultTo(math::TVector2<Element>(0, 0));
		}
	}

	inline std::string valueAsString(const math::TVector2<Element>& vec2) const override
	{
		return vec2.toString();
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		// TODO: view
		this->setValue(owner, sdl::load_vector2<Element>(std::string(payload.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		if(const math::TVector2<Element>* const vec2 = this->getValue(owner); vec2)
		{
			sdl::save_vector2<Element>(*vec2, &out_payload.value);
		}
	}
};

template<typename Owner, typename Element>
using TSdlOptionalVector2 = TSdlVector2<Owner, Element, TSdlOptionalValue<math::TVector2<Element>, Owner>>;

template<typename Owner>
using TSdlVector2R = TSdlVector2<Owner, real>;

template<typename Owner>
using TSdlVector2S = TSdlVector2<Owner, std::size_t>;

template<typename Owner>
using TSdlOptionalVector2R = TSdlOptionalVector2<Owner, real>;

template<typename Owner>
using TSdlOptionalVector2S = TSdlOptionalVector2<Owner, std::size_t>;

}// end namespace ph
