#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlIOUtils.h"

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
inline bool TSdlVector3<Owner, Element>::loadFromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	std::string&       out_loaderMessage)
{
	std::string parserMsg;
	auto optionalVector3 = SdlIOUtils::loadVector3R(sdlValue, &parserMsg);

	if(optionalVector3)
	{
		setValue(owner, std::move(*optionalVector3));
		return true;
	}

	return standardFailedLoadHandling(owner, parserMsg, out_loaderMessage);
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
