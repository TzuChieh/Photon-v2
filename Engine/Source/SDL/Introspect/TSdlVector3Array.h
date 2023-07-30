#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlResourceLocator.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"

#include <type_traits>
#include <string>
#include <vector>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<std::vector<math::TVector3<Element>>, Owner>>
class TSdlVector3Array : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<std::vector<math::TVector3<Element>>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

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
			math::TVector3<Element>* const vec3Data = vec3Vec->data();
			data = SdlNativeData(
				[vec3Data](std::size_t elementIdx) -> SdlNativeData::GetterVariant
				{
					const auto vec3Idx = elementIdx / 3;
					const auto compIdx = elementIdx - vec3Idx * 3;
					return SdlNativeData::permissiveElementToGetterVariant(&(vec3Data[vec3Idx][compIdx]));
				},
				[vec3Data](std::size_t elementIdx, SdlNativeData::SetterVariant input) -> bool
				{
					const auto vec3Idx = elementIdx / 3;
					const auto compIdx = elementIdx - vec3Idx * 3;
					return SdlNativeData::permissiveSetterVariantToElement(input, &(vec3Data[vec3Idx][compIdx]));
				},
				AnyNonConstPtr(vec3Vec));

			data.numElements = vec3Vec->size() * 3;
		}
		data.elementContainer = ESdlDataFormat::Vector3Vector;
		data.elementType = sdl::number_type_of<Element>();
		data.tupleSize = 3;
		return data;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		// TODO: resource file

		this->setValue(owner, sdl::load_vector3_array(std::string(clause.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		// TODO: resource file

		if(const std::vector<math::TVector3<Element>>* const vec3Arr = this->getConstValue(owner); vec3Arr)
		{
			sdl::save_vector3_array(*vec3Arr, &out_clause.value);
		}
	}
};

}// end namespace ph
