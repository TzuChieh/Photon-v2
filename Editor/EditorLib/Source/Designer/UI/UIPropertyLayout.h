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
	void clear();

	auto begin() noexcept -> typename std::vector<UIPropertyGroup>::iterator;
	auto begin() const noexcept -> typename std::vector<UIPropertyGroup>::const_iterator;
	auto end() noexcept -> typename std::vector<UIPropertyGroup>::iterator;
	auto end() const noexcept -> typename std::vector<UIPropertyGroup>::const_iterator;

private:
	std::vector<UIPropertyGroup> m_groups;
};

inline auto UIPropertyLayout::begin() noexcept
-> typename std::vector<UIPropertyGroup>::iterator
{
	return m_groups.begin();
}

inline auto UIPropertyLayout::begin() const noexcept
-> typename std::vector<UIPropertyGroup>::const_iterator
{
	return m_groups.begin();
}

inline auto UIPropertyLayout::end() noexcept
-> typename std::vector<UIPropertyGroup>::iterator
{
	return m_groups.end();
}

inline auto UIPropertyLayout::end() const noexcept
-> typename std::vector<UIPropertyGroup>::const_iterator
{
	return m_groups.end();
}

}// end namespace ph::editor
