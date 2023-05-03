#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
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

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		std::vector<math::TVector3<Element>>* const vec3Vec = this->getValue(owner);

		SdlNativeData data;
		if(vec3Vec)
		{
			data = SdlNativeData(
				[vec3Vec](const std::size_t elementIdx) -> void*
				{
					const auto vec3Idx = elementIdx / 3;
					return &((*vec3Vec)[vec3Idx]);
				},
				vec3Vec->size() * 3);
		}
		
		data.format = ESdlDataFormat::Vector3Array;
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
		this->setValue(owner, sdl::load_vector3_array(std::string(payload.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		if(const std::vector<math::TVector3<Element>>* const vec3Arr = this->getConstValue(owner); vec3Arr)
		{
			sdl::save_field_id(this, out_payload);
			sdl::save_vector3_array(*vec3Arr, &out_payload.value);
		}
	}
};

}// end namespace ph
