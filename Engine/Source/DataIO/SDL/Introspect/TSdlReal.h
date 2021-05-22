#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/SdlIOUtils.h"

#include <type_traits>
#include <string>
#include <utility>
#include <exception>

namespace ph
{

// TODO: change to TSdlFloat and add TSdlReal alias
template<typename Owner, typename RealType = real>
class TSdlReal : public TSdlValue<RealType, Owner>
{
	static_assert(std::is_same_v<RealType, real>, 
		"Currently supports only ph::real");

public:
	TSdlReal(std::string valueName, RealType Owner::* valuePtr);

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
inline TSdlReal<Owner, RealType>::TSdlReal(std::string valueName, RealType Owner::* const valuePtr) : 
	TSdlValue<RealType, Owner>("real", std::move(valueName), valuePtr)
{}

template<typename Owner, typename RealType>
inline std::string TSdlReal<Owner, RealType>::valueToString(const Owner& owner) const
{
	return std::to_string(getValue(owner));
}

template<typename Owner, typename RealType>
inline void TSdlReal<Owner, RealType>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	setValue(owner, SdlIOUtils::loadReal(sdlValue));
}

template<typename Owner, typename RealType>
inline void TSdlReal<Owner, RealType>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
