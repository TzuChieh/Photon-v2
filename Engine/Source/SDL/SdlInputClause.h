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

	/*! @brief If the carried value is a SDL reference.
	Identify the format of the string only. Does not test whether the reference actually 
	points to a valid target or not.
	*/
	bool isReference = false;

	// TODO: update to string_view
	// TODO: support binary data

public:
	inline SdlInputClause() = default;
	explicit SdlInputClause(std::string value);
	SdlInputClause(std::string value, std::string tag);
	SdlInputClause(std::string type, std::string name, std::string value);
	SdlInputClause(std::string type, std::string name, std::string value, std::string tag);

	/*! @brief Helper to check if the carried value is a SDL resource identifier (SRI).
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

inline std::string SdlInputClause::genPrettyName() const
{
	return "type: " + type + ", name: " + name;
}

inline std::string SdlInputClause::toString() const
{
	return "type: " + type + ", name: " + name + ", value: " + value + (tag.empty() ? "" : ", tag: " + tag);
}

}// end namespace ph
