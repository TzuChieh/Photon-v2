#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"

#include <type_traits>
#include <string>
#include <vector>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<std::vector<math::TVector3<Element>>, Owner>>
class TSdlVector3Array : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<std::vector<math::TVector3<Element>>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	template<typename ValueType>
	inline TSdlVector3Array(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("vector3-array", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const std::vector<math::TVector3<Element>>& vec3Array) const override
	{
		return "[" + std::to_string(vec3Array.size()) + " vector3 values...]";
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_vector3_array(std::string(payload.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
};

template<typename Owner, typename Element = real>
using TSdlOptionalVector3Array = TSdlVector3Array<Owner, Element, TSdlOptionalValue<std::vector<math::TVector3<Element>>, Owner>>;

}// end namespace ph
