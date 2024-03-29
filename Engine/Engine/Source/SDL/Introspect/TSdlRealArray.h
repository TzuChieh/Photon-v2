#pragma once

#include "SDL/Introspect/TSdlValue.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlResourceLocator.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "DataIO/io_utils.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/io_exceptions.h>

#include <type_traits>
#include <string>
#include <vector>
#include <utility>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<std::vector<Element>, Owner>>
class TSdlRealArray : public SdlValueType
{
	static_assert(std::is_base_of_v<TSdlAbstractValue<std::vector<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TSdlAbstractValue.");

public:
	template<typename ValueType>
	inline TSdlRealArray(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("real-array", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const std::vector<Element>& realArray) const override
	{
		return "[" + std::to_string(realArray.size()) + " real values...]";
	}

	inline SdlNativeData ownedNativeData(Owner& owner) const override
	{
		std::vector<Element>* const vec = this->getValue(owner);

		SdlNativeData data;
		if(vec)
		{
			Element* const vecData = vec->data();
			data = SdlNativeData(
				[vecData](std::size_t elementIdx) -> SdlGetterVariant
				{
					return SdlNativeData::permissiveElementGetter(&(vecData[elementIdx]));
				},
				[vecData](std::size_t elementIdx, SdlSetterVariant input) -> bool
				{
					return SdlNativeData::permissiveElementSetter(input, &(vecData[elementIdx]));
				},
				AnyNonConstPtr(vec));

			data.numElements = vec->size();
		}
		data.elementContainer = ESdlDataFormat::Vector;
		data.elementType = sdl::float_type_of<Element>();
		return data;
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override
	{
		if(clause.isResourceIdentifier())
		{
			try
			{
				const std::string loadedSdlValue = io_utils::load_text(
					SdlResourceLocator(ctx).toPath(clause.value));
				this->setValue(owner, sdl::load_number_array<Element>(loadedSdlValue));
			}
			catch(const FileIOError& e)
			{
				throw SdlLoadError("on loading real array -> " + e.whatStr());
			}
		}
		else
		{
			this->setValue(owner, sdl::load_number_array<Element>(clause.value));
		}
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override
	{
		// TODO: optionally as file

		if(const std::vector<Element>* const numberArr = this->getConstValue(owner); numberArr)
		{
			sdl::save_number_array<Element>(*numberArr, out_clause.value);
		}
	}
};

}// end namespace ph
