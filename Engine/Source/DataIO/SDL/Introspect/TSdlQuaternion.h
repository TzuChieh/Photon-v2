#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Math/TQuaternion.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <type_traits>
#include <string>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<math::TQuaternion<Element>, Owner>>
class TSdlQuaternion : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<math::TQuaternion<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	template<typename ValueType>
	inline TSdlQuaternion(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("quaternion", std::move(valueName), valuePtr)
	{
		if constexpr(std::is_same_v<SdlValueType, TSdlValue<math::TQuaternion<Element>, Owner>>)
		{
			this->defaultTo(math::TQuaternion<Element>::makeNoRotation());
		}
	}
	
	inline std::string valueAsString(const math::TQuaternion<Element>& quat) const override
	{
		return quat.toString();
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlPayload&      payload,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_quaternion(std::string(payload.value)));
	}

	inline void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override
	{
		PH_ASSERT(out_sdlValue);

		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
};

template<typename Owner, typename Element = real>
using TSdlOptionalQuaternion = TSdlQuaternion<Owner, Element, TSdlOptionalValue<math::TQuaternion<Element>, Owner>>;

}// end namespace ph
