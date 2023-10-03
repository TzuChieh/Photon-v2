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
		if constexpr(std::is_base_of_v<TSdlOptionalValue<math::TQuaternion<Element>, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optQuat = this->valueRef(owner)](std::size_t elementIdx) -> SdlGetterVariant
				{
					if(optQuat)
					{
						switch(elementIdx)
						{
						case 0: return SdlNativeData::permissiveElementGetter(&(optQuat->x));
						case 1: return SdlNativeData::permissiveElementGetter(&(optQuat->y));
						case 2: return SdlNativeData::permissiveElementGetter(&(optQuat->z));
						case 3: return SdlNativeData::permissiveElementGetter(&(optQuat->w));
						default: return std::monostate{};
						}
					}
					else
					{
						return std::monostate{};
					}
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
						
						switch(elementIdx)
						{
						case 0: return SdlNativeData::permissiveElementSetter(input, &(optQuat->x));
						case 1: return SdlNativeData::permissiveElementSetter(input, &(optQuat->y));
						case 2: return SdlNativeData::permissiveElementSetter(input, &(optQuat->z));
						case 3: return SdlNativeData::permissiveElementSetter(input, &(optQuat->w));
						default: return false;
						}
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
						switch(elementIdx)
						{
						case 0: return SdlNativeData::permissiveElementGetter(&(quat->x));
						case 1: return SdlNativeData::permissiveElementGetter(&(quat->y));
						case 2: return SdlNativeData::permissiveElementGetter(&(quat->z));
						case 3: return SdlNativeData::permissiveElementGetter(&(quat->w));
						default: return std::monostate{};
						}
					},
					[quat](std::size_t elementIdx, SdlSetterVariant input) -> bool
					{
						switch(elementIdx)
						{
						case 0: return SdlNativeData::permissiveElementSetter(input, &(quat->x));
						case 1: return SdlNativeData::permissiveElementSetter(input, &(quat->y));
						case 2: return SdlNativeData::permissiveElementSetter(input, &(quat->z));
						case 3: return SdlNativeData::permissiveElementSetter(input, &(quat->w));
						default: return false;
						}
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
		if(const math::TQuaternion<Element>* quat = this->getConstValue(owner); quat)
		{
			sdl::save_quaternion(*quat, &out_clause.value);
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
