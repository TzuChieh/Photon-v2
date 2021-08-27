#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <type_traits>
#include <string>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<math::TVector3<Element>, Owner>>
class TSdlVector3 : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<math::TVector3<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	/*! @brief Bind a Vector3 field to SDL.
	@tparam ValueType Type of the field member of the @p Owner class. The actual type
	requried depends on the @p SdlValueType used.
	*/
	template<typename ValueType>
	inline TSdlVector3(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("vector3", std::move(valueName), valuePtr)
	{
		if constexpr(std::is_same_v<SdlValueType, TSdlValue<math::TVector3<Element>, Owner>>)
		{
			this->defaultTo(math::TVector3<Element>(0, 0, 0));
		}
	}

	inline std::string valueAsString(const math::TVector3<Element>& vec3) const override
	{
		return vec3.toString();
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		// TODO: view
		this->setValue(owner, sdl::load_vector3(std::string(payload.value)));
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

/*! @brief SDL binding type of an optional Vector3.
*/
template<typename Owner, typename Element = real>
using TSdlOptionalVector3 = TSdlVector3<Owner, Element, TSdlOptionalValue<math::TVector3<Element>, Owner>>;

}// end namespace ph
