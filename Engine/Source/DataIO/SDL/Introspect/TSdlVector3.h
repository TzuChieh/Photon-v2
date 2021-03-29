#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

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



}// end namespace ph
