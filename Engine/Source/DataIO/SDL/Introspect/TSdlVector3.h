#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "DataIO/SDL/SdlIOUtils.h"

#include <type_traits>
#include <string>
#include <exception>

namespace ph
{

template<typename Owner, typename Element>
class TSdlVector3 : public TSdlValue<math::TVector3<Element>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlVector3(std::string valueName, math::TVector3<Element> Owner::* valuePtr);

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

template<typename Owner, typename Element>
inline TSdlVector3<Owner, Element>::TSdlVector3(std::string valueName, math::TVector3<Element> Owner::* const valuePtr) :
	TSdlValue<math::TVector3<Element>, Owner>("vector3", std::move(valueName), valuePtr)
{}

template<typename Owner, typename Element>
inline std::string TSdlVector3<Owner, Element>::valueToString(const Owner& owner) const
{
	return getValue(owner).toString();
}

template<typename Owner, typename Element>
inline void TSdlVector3<Owner, Element>::loadFromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	SdlInputContext&   ctx)
{
	setValue(owner, SdlIOUtils::loadVector3R(sdlValue));
}

template<typename Owner, typename Element>
void TSdlVector3<Owner, Element>::convertToSdl(
	Owner&       owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
