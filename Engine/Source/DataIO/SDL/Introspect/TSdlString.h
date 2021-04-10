#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <string>

namespace ph
{

// TODO: test
template<typename Owner>
class TSdlString : public TSdlValue<std::string, Owner>
{
public:
	TSdlString(std::string valueName, std::string Owner::* valuePtr);

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

template<typename Owner>
inline TSdlString<Owner>::TSdlString(std::string valueName, std::string Owner::* const valuePtr) :
	TSdlValue<std::string, Owner>("string", std::move(valueName), valuePtr)
{}

template<typename Owner>
inline std::string TSdlString<Owner>::valueToString(const Owner& owner) const
{
	return getValue(owner);
}

template<typename Owner>
inline void TSdlString<Owner>::loadFromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	SdlInputContext&   ctx)
{
	// Save <sdlValue> directly as it is already a string
	setValue(owner, sdlValue);
}

template<typename Owner>
inline void TSdlString<Owner>::convertToSdl(
	const Owner& owner,
	std::string* out_sdl,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdl);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
