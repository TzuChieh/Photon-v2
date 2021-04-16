#pragma once

#include <string_view>

namespace ph
{

class StringUtils final
{
public:
	static std::string_view trim(std::string_view srcStr);
};

// In-header Implementations:

inline std::string_view StringUtils::trim(const std::string_view srcStr)
{
	// TODO
}

}// end namespace ph
