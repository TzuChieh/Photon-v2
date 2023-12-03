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
		if constexpr(std::is_base_of_v<TSdlOptionalValue<math::TVector3<Element>, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optVec3 = this->valueRef(owner)](std::size_t elementIdx) -> SdlGetterVariant
				{
					return optVec3
						? SdlNativeData::permissiveElementGetter(&((*optVec3)[elementIdx]))
						: std::monostate{};
				},
				[&optVec3 = this->valueRef(owner)](std::size_t elementIdx, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optVec3 = std::nullopt;
						return true;
					}
					else
					{
						if(!optVec3)
						{
							optVec3 = math::TVector3<Element>{};
						}
						
						return SdlNativeData::permissiveElementSetter(input, &((*optVec3)[elementIdx]));
					}
				},
				AnyNonConstPtr(vec3));

			data.elementContainer = ESdlDataFormat::Vector3;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = vec3 ? 3 : 0;
			data.tupleSize = 3;
			data.isNullClearable = true;
			return data;
		}
		else
		{
			SdlNativeData data;
			if(vec3)
			{
				data = SdlNativeData(
					[vec3](std::size_t elementIdx) -> SdlGetterVariant
					{
						return SdlNativeData::permissiveElementGetter(&((*vec3)[elementIdx]));
					},
					[vec3](std::size_t elementIdx, SdlSetterVariant input) -> bool
					{
						return SdlNativeData::permissiveElementSetter(input, &((*vec3)[elementIdx]));
					},
					AnyNonConstPtr(vec3));
			}
			data.elementContainer = ESdlDataFormat::Vector3;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = vec3 ? 3 : 0;
			data.tupleSize = 3;
			return data;
		}
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
		if(const math::TVector3<Element>* vec3 = this->getConstValue(owner); vec3)
		{
			sdl::save_vector3(*vec3, &out_clause.value);
		}
		else
		{
			out_clause.isEmpty = true;
		}
	}
};

/*! @brief SDL binding type of an optional Vector3.
*/
template<typename Owner, typename Element = real>
using TSdlOptionalVector3 = TSdlVector3<Owner, Element, TSdlOptionalValue<math::TVector3<Element>, Owner>>;

}// end namespace ph
