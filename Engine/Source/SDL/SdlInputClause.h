#pragma once

#include "SDL/sdl_helpers.h"

#include <string>
#include <string_view>

namespace ph
{

/*! @brief Carries SDL representation of various data during the input process.
Helps to read input data such as command clauses or data packets.
*/
class SdlInputClause final
{
public:
	std::string type;
	std::string name;
	std::string value;
	std::string tag;

	// TODO: update to string_view
	// TODO: support binary data

public:
	inline SdlInputClause() = default;
	explicit SdlInputClause(std::string value);
	SdlInputClause(std::string value, std::string tag);
	SdlInputClause(std::string type, std::string name, std::string value);
	SdlInputClause(std::string type, std::string name, std::string value, std::string tag);

	/*! @brief Helper to check if the carried value is a SDL reference.
	*/
	bool isReference() const;

	/*! @brief Helper to check if the carried value is a SDL resource identifier.
	*/
	bool isResourceIdentifier() const;

	std::string genPrettyName() const;
	std::string toString() const;
};

inline SdlInputClause::SdlInputClause(std::string value) :
	SdlInputClause(std::move(value), "")
{}

inline SdlInputClause::SdlInputClause(std::string value, std::string tag) :
	SdlInputClause("", "", std::move(value), std::move(tag))
{}

inline SdlInputClause::SdlInputClause(std::string type, std::string name, std::string value) :
	SdlInputClause(std::move(type), std::move(name), std::move(value), "")
{}

inline SdlInputClause::SdlInputClause(
	std::string type, 
	std::string name, 
	std::string value, 
	std::string tag)
	
	: type(std::move(type))
	, name(std::move(name))
	, value(std::move(value))
	, tag(std::move(tag))
{}


inline bool SdlInputClause::isReference() const
{
	return sdl::is_reference(value);
}

inline bool SdlInputClause::isResourceIdentifier() const
{
	return sdl::is_resource_identifier(value);
}

inline std::string SdlInputClause::genPrettyName() const
{
	return "type: " + type + ", name: " + name;
}

inline std::string SdlInputClause::toString() const
{
	return "type: " + type + ", name: " + name + ", value: " + value + (tag.empty() ? "" : ", tag: " + tag);
}

}// end namespace ph
