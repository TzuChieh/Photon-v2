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
	std::string value;
	std::string tag;

public:
	SdlOutputPayload();

	std::string toString() const;
};

// In-header Implementations:

inline SdlOutputPayload::SdlOutputPayload() :
	value(""), tag("")
{}

inline std::string SdlOutputPayload::toString() const
{
	return "value: " + std::string(value) + (tag.empty() ? "" : ", tag: " + std::string(tag));
}

}// end namespace ph
