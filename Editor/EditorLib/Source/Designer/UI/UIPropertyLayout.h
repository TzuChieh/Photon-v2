#pragma once

#include "Designer/UI/UIPropertyGroup.h"

#include <vector>

namespace ph::editor
{

class UIPropertyLayout final
{
public:
	UIPropertyLayout& addGroup(UIPropertyGroup group);

private:
	std::vector<UIPropertyGroup> m_groups;
};

}// end namespace ph::editor
