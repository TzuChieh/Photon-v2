#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/SdlIOUtils.h"
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
template<typename Owner, typename RealType = real>
class TSdlRealArray : public TSdlValue<std::vector<RealType>, Owner>
{
	static_assert(std::is_same_v<RealType, real>,
		"Currently supports only ph::real");

public:
	TSdlRealArray(std::string valueName, std::vector<RealType> Owner::* valuePtr);

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

template<typename Owner, typename RealType>
inline TSdlRealArray<Owner, RealType>::TSdlRealArray(std::string valueName, std::vector<RealType> Owner::* const valuePtr) :
	TSdlValue<std::vector<RealType>, Owner>("real-array", std::move(valueName), valuePtr)
{}

template<typename Owner, typename RealType>
inline std::string TSdlRealArray<Owner, RealType>::valueToString(const Owner& owner) const
{
	return "[" + std::to_string(getValue(owner).size()) + " real values...]";
}

template<typename Owner, typename RealType>
inline void TSdlRealArray<Owner, RealType>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	if(SdlIOUtils::isResourceIdentifier(sdlValue))
	{
		const SdlResourceIdentifier sdlResId(sdlValue, ctx.workingDirectory);

		try
		{
			std::string loadedSdlValue = io_utils::load_text(sdlResId.getPathToResource());

			setValue(
				owner, 
				SdlIOUtils::loadRealArray(std::move(loadedSdlValue)));
		}
		catch(const FileIOError& e)
		{
			throw SdlLoadError("on loading real array -> " + e.whatStr());
		}
	}
	else
	{
		setValue(owner, SdlIOUtils::loadRealArray(sdlValue));
	}
}

template<typename Owner, typename RealType>
void TSdlRealArray<Owner, RealType>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
