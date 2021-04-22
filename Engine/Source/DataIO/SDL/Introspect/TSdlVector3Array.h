#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/SdlIOUtils.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"

#include <type_traits>
#include <string>
#include <exception>
#include <vector>

namespace ph
{

template<typename Owner, typename RealType>
class TSdlVector3Array : public TSdlValue<std::vector<math::TVector3<RealType>>, Owner>
{
	static_assert(std::is_same_v<Element, real>,
		"Currently supports only ph::real");

public:
	TSdlVector3Array(std::string valueName, std::vector<math::TVector3<RealType>> Owner::* valuePtr);

	std::string valueToString(const Owner& owner) const override;

private:
	void loadFromSdl(
		Owner&             owner,
		const std::string& sdlValue,
		SdlInputContext&   ctx) const override;

	void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override;
};

// In-header Implementations:

template<typename Owner, typename RealType>
inline TSdlVector3Array<Owner, RealType>::TSdlVector3Array(
	std::string valueName, 
	std::vector<math::TVector3<RealType>> Owner::* const valuePtr) :

	TSdlValue<std::vector<RealType>, Owner>("real-array", std::move(valueName), valuePtr)
{}

template<typename Owner, typename RealType>
inline std::string TSdlVector3Array<Owner, RealType>::valueToString(const Owner& owner) const
{
	return "[" + std::to_string(getValue(owner).size()) + " vector3 values...]";
}

template<typename Owner, typename RealType>
inline void TSdlVector3Array<Owner, RealType>::loadFromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	SdlInputContext&   ctx) const
{
	setValue(owner, SdlIOUtils::loadVector3RArray(sdlValue));
}

template<typename Owner, typename RealType>
void TSdlVector3Array<Owner, RealType>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
