#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Common/assertion.h"
#include "SDL/sdl_helpers.h"

#include <type_traits>
#include <string>
#include <cstddef>

namespace ph
{

template<typename Owner, typename Element, typename SdlValueType = TSdlValue<math::TVector2<Element>, Owner>>
class TSdlVector2 : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<math::TVector2<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlVector2(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("vector2", std::move(valueName), valuePtr)
	{
		if constexpr(std::is_same_v<SdlValueType, TSdlValue<math::TVector2<Element>, Owner>>)
		{
			this->defaultTo(math::TVector2<Element>(0, 0));
		}
	}

	inline std::string valueAsString(const math::TVector2<Element>& vec2) const override
	{
		return vec2.toString();
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		math::TVector2<Element>* const vec2 = this->getValue(owner);
		if constexpr(std::is_base_of_v<TSdlOptionalValue<math::TVector2<Element>, Owner>, SdlValueType>)
		{
			auto data = SdlNativeData(
				[&optVec2 = this->valueRef(owner)](std::size_t elementIdx) -> SdlGetterVariant
				{
					return optVec2
						? SdlNativeData::permissiveElementGetter(&((*optVec2)[elementIdx]))
						: std::monostate{};
				},
				[&optVec2 = this->valueRef(owner)](std::size_t elementIdx, SdlSetterVariant input) -> bool
				{
					if(input.isEmpty())
					{
						optVec2 = std::nullopt;
						return true;
					}
					else
					{
						if(!optVec2)
						{
							optVec2 = math::TVector2<Element>{};
						}
						
						return SdlNativeData::permissiveElementSetter(input, &((*optVec2)[elementIdx]));
					}
				},
				AnyNonConstPtr(vec2));

			data.elementContainer = ESdlDataFormat::Vector2;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = vec2 ? 2 : 0;
			data.tupleSize = 2;
			data.isNullClearable = true;
			return data;
		}
		else
		{
			SdlNativeData data;
			if(vec2)
			{
				data = SdlNativeData(
					[vec2](std::size_t elementIdx) -> SdlGetterVariant
					{
						return SdlNativeData::permissiveElementGetter(&((*vec2)[elementIdx]));
					},
					[vec2](std::size_t elementIdx, SdlSetterVariant input) -> bool
					{
						return SdlNativeData::permissiveElementSetter(input, &((*vec2)[elementIdx]));
					},
					AnyNonConstPtr(vec2));
			}
			data.elementContainer = ESdlDataFormat::Vector2;
			data.elementType = sdl::number_type_of<Element>();
			data.numElements = vec2 ? 2 : 0;
			data.tupleSize = 2;
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
		this->setValue(owner, sdl::load_vector2<Element>(std::string(clause.value)));
	}

	inline void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		if(const math::TVector2<Element>* const vec2 = this->getConstValue(owner); vec2)
		{
			sdl::save_vector2<Element>(*vec2, &out_clause.value);
		}
	}
};

template<typename Owner, typename Element>
using TSdlOptionalVector2 = TSdlVector2<Owner, Element, TSdlOptionalValue<math::TVector2<Element>, Owner>>;

template<typename Owner>
using TSdlVector2R = TSdlVector2<Owner, real>;

template<typename Owner>
using TSdlVector2S = TSdlVector2<Owner, std::size_t>;

template<typename Owner>
using TSdlOptionalVector2R = TSdlOptionalVector2<Owner, real>;

template<typename Owner>
using TSdlOptionalVector2S = TSdlOptionalVector2<Owner, std::size_t>;

}// end namespace ph
