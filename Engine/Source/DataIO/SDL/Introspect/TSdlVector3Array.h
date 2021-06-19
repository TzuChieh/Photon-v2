#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"

#include <type_traits>
#include <string>
#include <exception>
#include <vector>

namespace ph
{

template<typename Owner, typename Element = real>
class TSdlVector3Array : public TSdlValue<std::vector<math::TVector3<Element>>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlVector3Array(std::string valueName, std::vector<math::TVector3<Element>> Owner::* valuePtr);

	inline std::string valueAsString(const std::vector<math::TVector3<Element>>& vec3Array) const override
	{
		return "[" + std::to_string(vec3Array.size()) + " vector3 values...]";
	}

protected:
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
inline TSdlVector3Array<Owner, Element>::TSdlVector3Array(
	std::string valueName, 
	std::vector<math::TVector3<Element>> Owner::* const valuePtr) :

	TSdlValue<std::vector<math::TVector3<Element>>, Owner>(
		"vector3-array", 
		std::move(valueName), 
		valuePtr)
{}

template<typename Owner, typename Element>
inline void TSdlVector3Array<Owner, Element>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	setValue(owner, sdl::load_vector3_array(sdlValue));
}

template<typename Owner, typename Element>
void TSdlVector3Array<Owner, Element>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
