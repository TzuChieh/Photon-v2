#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Math/TQuaternion.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"

#include <type_traits>
#include <string>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<math::TQuaternion<Element>, Owner>>
class TSdlQuaternion : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<math::TQuaternion<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

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

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		math::TQuaternion<Element>* const quat = this->getValue(owner);

		SdlNativeData data;
		if(quat)
		{
			data = SdlNativeData(
				[quat](const std::size_t elementIdx) -> void*
				{
					switch(elementIdx)
					{
					case 0: return &(quat->x);
					case 1: return &(quat->y);
					case 2: return &(quat->z);
					case 3: return &(quat->w);
					default: return nullptr;
					}
				},
				4);
		}

		data.format = ESdlDataFormat::Quaternion;
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
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_quaternion(std::string(payload.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		if(const math::TQuaternion<Element>* const quat = this->getConstValue(owner); quat)
		{
			sdl::save_field_id(this, out_payload);
			sdl::save_quaternion(*quat, &out_payload.value);
		}
	}
};

template<typename Owner, typename Element = real>
using TSdlOptionalQuaternion = TSdlQuaternion<Owner, Element, TSdlOptionalValue<math::TQuaternion<Element>, Owner>>;

}// end namespace ph
