#pragma once

#include <string_view>

namespace ph::editor
{

class AboutInformation final
{
public:
	AboutInformation();

	std::string_view authorInfo;
	std::string_view citationInfo;
};

}// end namespace ph::editor
