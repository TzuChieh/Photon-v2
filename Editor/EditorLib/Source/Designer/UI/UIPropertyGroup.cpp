#include "Designer/UI/UIPropertyGroup.h"

#include <utility>

namespace ph::editor
{

UIPropertyGroup& UIPropertyGroup::addProperty(UIProperty property)
{
	m_properties.push_back(std::move(property));

	return *this;
}

}// end namespace ph::editor
