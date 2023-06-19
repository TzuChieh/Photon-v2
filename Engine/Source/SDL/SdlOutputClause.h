#pragma once

#include "SDL/sdl_helpers.h"

#include <string>

namespace ph
{

/*! @brief Carries SDL representation of various data during the output process.
Helps to write output data such as command clauses or data packets.
*/
class SdlOutputClause final
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

	// TODO: support binary data

public:
	inline SdlOutputClause() = default;

	void clear();
	bool hasTag() const;

	std::string toString() const;
};

inline void SdlOutputClause::clear()
{
	type.clear();
	name.clear();
	value.clear();
	tag.clear();

	isReference = false;
}

inline bool SdlOutputClause::hasTag() const
{
	return !tag.empty();
}

inline std::string SdlOutputClause::toString() const
{
	return "type: " + type + ", name: " + name + ", value: " + value + (tag.empty() ? "" : ", tag: " + tag);
}

}// end namespace ph
