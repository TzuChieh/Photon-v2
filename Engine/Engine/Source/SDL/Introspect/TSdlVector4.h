#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Math/TVector4.h"
#include "SDL/sdl_helpers.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <type_traits>
#include <string>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<math::TVector4<Element>, Owner>>
class TSdlVector4 : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<math::TVector4<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	/*! @brief Bind a Vector4 field to SDL.
	@tparam ValueType Type of the field member of the @p Owner class. The actual type
	requried depends on the @p SdlValueType used.
	*/
	template<typename ValueType>
	TSdlVector4(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("vector4", std::move(valueName), valuePtr)
	{
		if constexpr(std::is_same_v<SdlValueType, TSdlValue<math::TVector4<Element>, Owner>>)
		{
			this->defaultTo(math::TVector4<Element>(0, 0, 0, 0));
		}
	}

	std::string valueAsString(const math::TVector4<Element>& vec4) const override
	{
		return vec4.toString();
	}

	SdlNativeData ownedNativeData(Owner& owner) const override
	{
		math::TVector4<Element>* const vec4 = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<math::TVector4<Element>, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optVec4 = this->valueRef(owner)](std::size_t elementIdx) -> SdlGetterVariant
				{
					return optVec4
						? SdlNativeData::permissiveElementGetter(&((*optVec4)[elementIdx]))
						: std::monostate{};
				},
				[&optVec4 = this->valueRef(owner)](std::size_t elementIdx, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optVec4 = std::nullopt;
						return true;
					}
					else
					{
						if(!optVec4)
						{
							optVec4 = math::TVector4<Element>{};
						}
						
						return SdlNativeData::permissiveElementSetter(input, &((*optVec4)[elementIdx]));
					}
				},
				AnyNonConstPtr(vec4));

			data.elementContainer = ESdlDataFormat::Vector4;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = vec4 ? 4 : 0;
			data.tupleSize = 4;
			data.isNullClearable = true;
			return data;
		}
		else
		{
			SdlNativeData data;
			if(vec4)
			{
				data = SdlNativeData(
					[vec4](std::size_t elementIdx) -> SdlGetterVariant
					{
						return SdlNativeData::permissiveElementGetter(&((*vec4)[elementIdx]));
					},
					[vec4](std::size_t elementIdx, SdlSetterVariant input) -> bool
					{
						return SdlNativeData::permissiveElementSetter(input, &((*vec4)[elementIdx]));
					},
					AnyNonConstPtr(vec4));
			}
			data.elementContainer = ESdlDataFormat::Vector4;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = vec4 ? 4 : 0;
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
		this->setValue(owner, sdl::load_vector4<Element>(clause.value));
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const math::TVector4<Element>* vec4 = this->getConstValue(owner); vec4)
		{
			sdl::save_vector4(*vec4, out_clause.value);
		}
		else
		{
			out_clause.isEmpty = true;
		}
	}
};

/*! @brief SDL binding type of an optional Vector4.
*/
template<typename Owner, typename Element = real>
using TSdlOptionalVector4 = TSdlVector4<Owner, Element, TSdlOptionalValue<math::TVector4<Element>, Owner>>;

}// end namespace ph
