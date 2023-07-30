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
				[quat](std::size_t elementIdx) -> SdlNativeData::GetterVariant
				{
					switch(elementIdx)
					{
					case 0: return SdlNativeData::permissiveElementToGetterVariant(&(quat->x));
					case 1: return SdlNativeData::permissiveElementToGetterVariant(&(quat->y));
					case 2: return SdlNativeData::permissiveElementToGetterVariant(&(quat->z));
					case 3: return SdlNativeData::permissiveElementToGetterVariant(&(quat->w));
					default: return std::monostate{};
					}
				},
				[quat](std::size_t elementIdx, SdlNativeData::SetterVariant input) -> bool
				{
					switch(elementIdx)
					{
					case 0: return SdlNativeData::permissiveSetterVariantToElement(input, &(quat->x));
					case 1: return SdlNativeData::permissiveSetterVariantToElement(input, &(quat->y));
					case 2: return SdlNativeData::permissiveSetterVariantToElement(input, &(quat->z));
					case 3: return SdlNativeData::permissiveSetterVariantToElement(input, &(quat->w));
					default: return false;
					}
				},
				AnyNonConstPtr(quat));
		}
		data.elementContainer = ESdlDataFormat::Quaternion;
		data.elementType = sdl::number_type_of<Element>();
		data.numElements = 4;
		data.tupleSize = 4;
		return data;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_quaternion(std::string(clause.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const math::TQuaternion<Element>* const quat = this->getConstValue(owner); quat)
		{
			sdl::save_quaternion(*quat, &out_clause.value);
		}
	}
};

template<typename Owner, typename Element = real>
using TSdlOptionalQuaternion = TSdlQuaternion<Owner, Element, TSdlOptionalValue<math::TQuaternion<Element>, Owner>>;

}// end namespace ph
