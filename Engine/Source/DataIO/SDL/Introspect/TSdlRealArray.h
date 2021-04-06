#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlIOUtils.h"

#include <type_traits>
#include <string>
#include <exception>
#include <vector>

namespace ph
{

template<typename Owner, typename RealType>
class TSdlRealArray : public TSdlValue<math::TQuaternion<std::vector<RealType>>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlRealArray(std::string valueName, math::TQuaternion<Element> Owner::* valuePtr);

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
	Owner&             owner,
	const std::string& sdlValue,
	SdlInputContext&   ctx)
{
	setValue(owner, SdlIOUtils::loadRealArray(sdlValue));
}

template<typename Owner, typename RealType>
void TSdlRealArray<Owner, RealType>::convertToSdl(
	Owner&       owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
