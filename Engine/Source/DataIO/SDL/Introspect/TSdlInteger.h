#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlIOUtils.h"

#include <type_traits>
#include <string>
#include <utility>
#include <exception>

namespace ph
{

template<typename Owner, typename IntType = integer>
class TSdlInteger : public TSdlValue<IntType, Owner>
{
	static_assert(std::is_same_v<IntType, integer>,
		"Currently supports only ph::integer");

public:
	TSdlInteger(std::string valueName, IntType Owner::* valuePtr);

	std::string valueToString(const Owner& owner) const override;

	void loadFromSdl(
		Owner&             owner,
		const std::string& sdlValue,
		SdlInputContext&   ctx) override;

	void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override;
};

// In-header Implementations:

template<typename Owner, typename IntType>
inline TSdlInteger<Owner, IntType>::TSdlInteger(std::string valueName, IntType Owner::* const valuePtr) :
	TSdlValue<IntType, Owner>("integer", std::move(valueName), valuePtr)
{}

template<typename Owner, typename IntType>
inline std::string TSdlInteger<Owner, IntType>::valueToString(const Owner& owner) const
{
	return std::to_string(getValue(owner));
}

template<typename Owner, typename IntType>
inline void TSdlInteger<Owner, IntType>::loadFromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	SdlInputContext&   ctx)
{
	setValue(owner, SdlIOUtils::loadInteger(sdlValue));
}

template<typename Owner, typename IntType>
inline void TSdlInteger<Owner, IntType>::convertToSdl(
	Owner&       owner,
	std::string* out_sdl,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
