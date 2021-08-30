#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "DataIO/SDL/Introspect/TSdlOptionalValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"

#include <type_traits>
#include <string>
#include <vector>
#include <utility>

namespace ph
{

template<typename Owner, typename Element = real, typename SdlValueType = TSdlValue<std::vector<Element>, Owner>>
class TSdlRealArray : public SdlValueType
{
	static_assert(std::is_base_of_v<TAbstractSdlValue<std::vector<Element>, Owner>, SdlValueType>,
		"SdlValueType should be a subclass of TAbstractSdlValue.");

	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	template<typename ValueType>
	inline TSdlRealArray(std::string valueName, ValueType Owner::* const valuePtr) :
		SdlValueType("real-array", std::move(valueName), valuePtr)
	{}

	inline std::string valueAsString(const std::vector<Element>& realArray) const override
	{
		return "[" + std::to_string(realArray.size()) + " real values...]";
	}

protected:
	inline void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override
	{
		if(payload.isResourceIdentifier())
		{
			const SdlResourceIdentifier sdlResId(payload.value, ctx.getWorkingDirectory());

			try
			{
				std::string loadedSdlValue = io_utils::load_text(sdlResId.getPathToResource());

				this->setValue(
					owner,
					sdl::load_real_array(std::move(loadedSdlValue)));
			}
			catch(const FileIOError& e)
			{
				throw SdlLoadError("on loading real array -> " + e.whatStr());
			}
		}
		else
		{
			this->setValue(owner, sdl::load_real_array(std::string(payload.value)));
		}
	}

	void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override
	{
		// TODO: optionally as file

		if(const std::vector<Element>* const realArr = this->getValue(owner); realArr)
		{
			sdl::save_field_id(this, out_payload);
			sdl::save_real_array(*realArr, &out_payload.value);
		}
	}
};

template<typename Owner, typename Element = real>
using TSdlOptionalRealArray = TSdlRealArray<Owner, Element, TSdlOptionalValue<std::vector<Element>, Owner>>;

}// end namespace ph
