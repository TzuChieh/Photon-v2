#pragma once

#include "DataIO/SDL/sdl_helpers.h"

#include <string>

namespace ph
{

/*! @brief Carries SDL representation of various data during the output process.
*/
class SdlOutputPayload final
{
public:
	std::string type;
	std::string name;
	std::string value;
	std::string tag;

public:
	SdlOutputPayload();

	void clear();

	std::string toString() const;
};

// In-header Implementations:

inline SdlOutputPayload::SdlOutputPayload() :
	type(""), name(""), value(""), tag("")
{}

inline void SdlOutputPayload::clear()
{
	type.clear();
	name.clear();
	value.clear();
	tag.clear();
}

inline std::string SdlOutputPayload::toString() const
{
	return "type: " + type + ", name: " + name + ", value: " + value + (tag.empty() ? "" : ", tag: " + tag);
}

}// end namespace ph
