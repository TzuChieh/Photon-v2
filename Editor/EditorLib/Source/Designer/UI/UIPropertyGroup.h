#pragma once

#include "Designer/UI/UIProperty.h"

#include <vector>
#include <string>

namespace ph::editor
{

class UIPropertyGroup final
{
public:
	UIPropertyGroup& addProperty(UIProperty property);

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
