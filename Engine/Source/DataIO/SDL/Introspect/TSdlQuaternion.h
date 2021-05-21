#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Math/TQuaternion.h"
#include "Common/assertion.h"
#include "DataIO/SDL/SdlIOUtils.h"

#include <type_traits>
#include <string>
#include <exception>

namespace ph
{

template<typename Owner, typename Element = real>
class TSdlQuaternion : public TSdlValue<math::TQuaternion<Element>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlQuaternion(std::string valueName, math::TQuaternion<Element> Owner::* valuePtr);
	
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

template<typename Owner, typename Element>
inline TSdlQuaternion<Owner, Element>::TSdlQuaternion(std::string valueName, math::TQuaternion<Element> Owner::* const valuePtr) :
	TSdlValue<math::TQuaternion<Element>, Owner>("quaternion", std::move(valueName), valuePtr)
{
	defaultTo(math::TQuaternion<Element>::makeNoRotation());
}

template<typename Owner, typename Element>
inline std::string TSdlQuaternion<Owner, Element>::valueToString(const Owner& owner) const
{
	return getValue(owner).toString();
}

template<typename Owner, typename Element>
inline void TSdlQuaternion<Owner, Element>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	setValue(owner, SdlIOUtils::loadQuaternionR(sdlValue));
}

template<typename Owner, typename Element>
void TSdlQuaternion<Owner, Element>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
