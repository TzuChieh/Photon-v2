#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"

#include <type_traits>
#include <string>
#include <exception>
#include <vector>
#include <utility>

namespace ph
{

// TODO: change to TSdlFloatArray and add TSdlRealArray alias
template<typename Owner, typename Element = real>
class TSdlRealArray : public TSdlValue<std::vector<Element>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlRealArray(std::string valueName, std::vector<Element> Owner::* valuePtr);

	std::string valueToString(const Owner& owner) const override;

private:
	void loadFromSdl(
		Owner&                 owner,
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override;

	void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override;
};

// In-header Implementations:

template<typename Owner, typename Element>
inline TSdlRealArray<Owner, Element>::TSdlRealArray(
	std::string valueName, 
	std::vector<Element> Owner::* const valuePtr) :

	TSdlValue<std::vector<Element>, Owner>(
		"real-array", 
		std::move(valueName), 
		valuePtr)
{}

template<typename Owner, typename Element>
inline std::string TSdlRealArray<Owner, Element>::valueToString(const Owner& owner) const
{
	return "[" + std::to_string(getValue(owner).size()) + " real values...]";
}

template<typename Owner, typename Element>
inline void TSdlRealArray<Owner, Element>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
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

template<typename Owner, typename Element>
void TSdlRealArray<Owner, Element>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
