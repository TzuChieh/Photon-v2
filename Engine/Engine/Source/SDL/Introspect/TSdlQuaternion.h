#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Math/TQuaternion.h"
#include "SDL/sdl_helpers.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

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
	TSdlQuaternion(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("quaternion", std::move(valueName), valuePtr)
	{
		if constexpr(std::is_same_v<SdlValueType, TSdlValue<math::TQuaternion<Element>, Owner>>)
		{
			this->defaultTo(math::TQuaternion<Element>::makeNoRotation());
		}
	}
	
	std::string valueAsString(const math::TQuaternion<Element>& quat) const override
	{
		return quat.toString();
	}

	SdlNativeData ownedNativeData(Owner& owner) const override
	{
		math::TQuaternion<Element>* const quat = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<math::TQuaternion<Element>, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optQuat = this->valueRef(owner)](std::size_t elementIdx) -> SdlGetterVariant
				{
					return optQuat
						? SdlNativeData::permissiveElementGetter(&((*optQuat)[elementIdx]))
						: std::monostate{};
				},
				[&optQuat = this->valueRef(owner)](std::size_t elementIdx, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optQuat = std::nullopt;
						return true;
					}
					else
					{
						if(!optQuat)
						{
							optQuat = math::TQuaternion<Element>{};
						}

						return SdlNativeData::permissiveElementSetter(input, &((*optQuat)[elementIdx]));
					}
				},
				AnyNonConstPtr(quat));

			data.elementContainer = ESdlDataFormat::Quaternion;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = quat ? 4 : 0;
			data.tupleSize = 4;
			data.isNullClearable = true;
			return data;
		}
		else
		{
			SdlNativeData data;
			if(quat)
			{
				data = SdlNativeData(
					[quat](std::size_t elementIdx) -> SdlGetterVariant
					{
						return SdlNativeData::permissiveElementGetter(&((*quat)[elementIdx]));
					},
					[quat](std::size_t elementIdx, SdlSetterVariant input) -> bool
					{
						return SdlNativeData::permissiveElementSetter(input, &((*quat)[elementIdx]));
					},
					AnyNonConstPtr(quat));
			}
			data.elementContainer = ESdlDataFormat::Quaternion;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = quat ? 4 : 0;
			data.tupleSize = 4;
			return data;
		}
	}

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		this->setValue(owner, sdl::load_quaternion<Element>(clause.value));
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const math::TQuaternion<Element>* quat = this->getConstValue(owner); quat)
		{
			sdl::save_quaternion(*quat, out_clause.value);
		}
		else
		{
			out_clause.isEmpty = true;
		}
	}
};

template<typename Owner, typename Element = real>
using TSdlOptionalQuaternion = TSdlQuaternion<Owner, Element, TSdlOptionalValue<math::TQuaternion<Element>, Owner>>;

}// end namespace ph
