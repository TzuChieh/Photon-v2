#pragma once

#include "DataIO/SDL/sdl_helpers.h"

#include <string>
#include <string_view>

namespace ph
{

/*! @brief Carries SDL representation of various data.
*/
class SdlPayload
{
public:
	/*std::string_view value;
	std::string_view tag;*/
	std::string value;
	std::string tag;

public:
	inline SdlPayload() = default;
	/*explicit SdlPayload(std::string_view value);
	SdlPayload(std::string_view value, std::string_view tag);*/
	explicit SdlPayload(std::string value);
	SdlPayload(std::string value, std::string tag);

	/*! @brief Helper to check if the carried value is a SDL reference.
	*/
	bool isReference() const;

	/*! @brief Helper to check if the carried value is a SDL resource identifier.
	*/
	bool isResourceIdentifier() const;

	std::string toString() const;
};

// In-header Implementations:

//inline SdlPayload::SdlPayload(std::string_view value) :
//	SdlPayload(std::move(value), "")
//{}
//
//inline SdlPayload::SdlPayload(std::string_view value, std::string_view tag) :
//	value(std::move(value)), tag(std::move(tag))
//{}

inline SdlPayload::SdlPayload(std::string value) :
	SdlPayload(std::move(value), "")
{}

inline SdlPayload::SdlPayload(std::string value, std::string tag) :
	value(std::move(value)), tag(std::move(tag))
{}

inline std::string SdlPayload::toString() const
{
	return "value: " + std::string(value) + (tag.empty() ? "" : ", tag: " + std::string(tag));
}

inline bool SdlPayload::isReference() const
{
	return sdl::is_reference(value);
}

inline bool SdlPayload::isResourceIdentifier() const
{
	return sdl::is_resource_identifier(value);
}

}// end namespace ph
