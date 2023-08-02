#include "Designer/UI/UIPropertyLayout.h"

#include <utility>

namespace ph::editor
{

UIPropertyLayout& UIPropertyLayout::addGroup(UIPropertyGroup group)
{
	m_groups.push_back(std::move(group));

	return *this;
}

}// end namespace ph::editor
