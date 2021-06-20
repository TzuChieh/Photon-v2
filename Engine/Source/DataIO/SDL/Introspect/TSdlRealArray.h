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
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override
	{
		if(sdl::is_resource_identifier(sdlValue))
		{
			const SdlResourceIdentifier sdlResId(sdlValue, ctx.workingDirectory);

			try
			{
				std::string loadedSdlValue = io_utils::load_text(sdlResId.getPathToResource());

				setValue(
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
			setValue(owner, sdl::load_real_array(sdlValue));
		}
	}

	void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override
	{
		PH_ASSERT(out_sdlValue);

		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}
};

template<typename Owner, typename Element = real>
using TSdlOptionalRealArray = TSdlRealArray<Owner, Element, TSdlOptionalValue<std::vector<Element>, Owner>>;

}// end namespace ph
