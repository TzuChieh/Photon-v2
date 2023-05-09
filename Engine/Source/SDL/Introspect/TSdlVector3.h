#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"

#include <type_traits>
#include <string>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<math::TVector3<Element>, Owner>>
class TSdlVector3 : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<math::TVector3<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

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

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		math::TVector3<Element>* const vec3 = this->getValue(owner);

		SdlNativeData data;
		if(vec3)
		{
			data = SdlNativeData(
				[vec3](const std::size_t elementIdx) -> void*
				{
					return &((*vec3)[elementIdx]);
				},
				3);
		}

		data.format = ESdlDataFormat::Vector3;
		if constexpr(std::is_floating_point_v<Element>)
		{
			data.dataType = sdl::float_type_of<Element>();
		}
		else
		{
			data.dataType = sdl::int_type_of<Element>();
		}

		return data;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		// TODO: view
		this->setValue(owner, sdl::load_vector3(std::string(clause.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const math::TVector3<Element>* const vec3 = this->getConstValue(owner); vec3)
		{
			sdl::save_field_id(this, out_clause);
			sdl::save_vector3(*vec3, &out_clause.value);
		}
	}
};

/*! @brief SDL binding type of an optional Vector3.
*/
template<typename Owner, typename Element = real>
using TSdlOptionalVector3 = TSdlVector3<Owner, Element, TSdlOptionalValue<math::TVector3<Element>, Owner>>;

}// end namespace ph
