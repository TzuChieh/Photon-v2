#pragma once

#include "Designer/UI/UIPropertyGroup.h"

#include <SDL/TSdlAnyInstance.h>

#include <vector>
#include <string>
#include <string_view>

namespace ph::editor
{

class UIPropertyLayout final
{
public:
	static UIPropertyLayout makeLinearListing(SdlNonConstInstance instance, std::string groupName = "");

	UIPropertyLayout& addGroup(UIPropertyGroup group, bool shouldPrepend = false);
	UIPropertyLayout& addGroups(const UIPropertyLayout& groups, bool shouldPrepend = false);
	UIPropertyGroup* getGroup(std::string_view groupName);
	UIPropertyGroup& getOrCreateGroup(std::string_view groupName, bool shouldPrepend = false);

private:
	std::vector<UIPropertyGroup> m_groups;
};

}// end namespace ph::editor
