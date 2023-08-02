#pragma once

#include "Designer/UI/UIProperty.h"

#include <vector>
#include <string>

namespace ph::editor
{

class UIPropertyGroup final
{
public:
	explicit UIPropertyGroup(std::string groupName);

	UIPropertyGroup& addProperty(UIProperty property, bool shouldPrepend = false);
	UIPropertyGroup& addProperties(const UIPropertyGroup& properties, bool shouldPrepend = false);
	const std::string& getName() const;

private:
	std::string m_name;
	std::vector<UIProperty> m_properties;
};

inline const std::string& UIPropertyGroup::getName() const
{
	return m_name;
}

}// end namespace ph::editor
