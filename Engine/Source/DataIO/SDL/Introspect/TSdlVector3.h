#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <type_traits>
#include <string>
#include <exception>

namespace ph
{

template<typename Owner, typename Element = real>
class TSdlVector3 : public TSdlValue<math::TVector3<Element>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlVector3(std::string valueName, math::TVector3<Element> Owner::* valuePtr);

	inline std::string valueAsString(const math::TVector3<Element>& vec3) const override
	{
		return vec3.toString();
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
inline TSdlVector3<Owner, Element>::TSdlVector3(std::string valueName, math::TVector3<Element> Owner::* const valuePtr) :
	TSdlValue<math::TVector3<Element>, Owner>("vector3", std::move(valueName), valuePtr)
{
	defaultTo(math::TVector3<Element>(0, 0, 0));
}

template<typename Owner, typename Element>
inline void TSdlVector3<Owner, Element>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	setValue(owner, sdl::load_vector3(sdlValue));
}

template<typename Owner, typename Element>
void TSdlVector3<Owner, Element>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
