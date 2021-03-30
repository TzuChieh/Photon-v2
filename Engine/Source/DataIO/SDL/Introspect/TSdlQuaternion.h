#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Math/TQuaternion.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlIOUtils.h"

#include <type_traits>
#include <string>
#include <exception>

namespace ph
{

template<typename Owner, typename Element>
class TSdlQuaternion : public TSdlValue<math::TQuaternion<Element>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlQuaternion(std::string valueName, math::TQuaternion<Element> Owner::* valuePtr);

	bool loadFromSdl(
		Owner&             owner,
		const std::string& sdlValue,
		std::string&       out_loaderMessage) override;

	void convertToSdl(
		Owner&       owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override;
};

// In-header Implementations:

template<typename Owner, typename Element>
inline TSdlQuaternion<Owner, Element>::TSdlQuaternion(std::string valueName, math::TQuaternion<Element> Owner::* const valuePtr) :
	TSdlValue<math::TQuaternion<Element>, Owner>("quaternion", std::move(valueName), valuePtr)
{}

template<typename Owner, typename Element>
inline bool TSdlQuaternion<Owner, Element>::loadFromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	std::string&       out_loaderMessage)
{
	std::string parserMsg;
	auto optionalQuat = SdlIOUtils::loadQuaternionR(sdlValue, &parserMsg);

	if(optionalQuat)
	{
		setValue(owner, std::move(*optionalQuat));
		return true;
	}

	return standardFailedLoadHandling(owner, parserMsg, out_loaderMessage);
}

template<typename Owner, typename Element>
void TSdlQuaternion<Owner, Element>::convertToSdl(
	Owner&       owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
