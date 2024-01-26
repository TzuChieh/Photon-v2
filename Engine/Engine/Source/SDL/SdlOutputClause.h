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

	/*! @brief Stores a name for the data.
	May refer to some output clauses. See `isUsingNamedOutputClauses`.
	*/
	std::string name;

	/*! @brief Stores stringified data of a clause.
	As the output clause generator knows best how its data look like, additional prefixes
	or suffixes may need to be added to ensure data integrity. For example, if a string
	contains whitespaces, double quotes should be used (e.g., " many spaces  in data ");
	for an array of quoted elements, curly braces can be used (e.g, {"a", "b", "c"}).
	*/
	std::string value;

	std::string tag;
	
	/*! @brief If the clause carries no data and does not need to be written.
	*/
	bool isEmpty = false;

	/*! @brief If the carried value is a SDL reference.
	Identify the format of the string only. Does not test whether the reference actually
	points to a valid target or not.
	*/
	bool isReference = false;

	/*! @brief If the carried name refers to some output clauses.
	*/
	bool isUsingNamedOutputClauses = false;

	// TODO: support binary data

public:
	SdlOutputClause() = default;

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

	isEmpty = false;
	isReference = false;
	isUsingNamedOutputClauses = false;
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
