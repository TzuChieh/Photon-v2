#pragma once

#include "Designer/UI/UIProperty.h"

#include <vector>
#include <string>
#include <string_view>

namespace ph::editor
{

class UIPropertyGroup final
{
public:
	explicit UIPropertyGroup(std::string groupName);

	UIPropertyGroup& addProperty(UIProperty property, bool shouldPrepend = false);
	UIPropertyGroup& addProperties(const UIPropertyGroup& properties, bool shouldPrepend = false);

	/*! @brief Find a property in group by its unique name.
	Properties in the same group should have unique field names.
	@param fieldName The SDL field name of the property.
	@return The property with `fieldName`. Null if not found.
	*/
	UIProperty* findProperty(std::string_view fieldName);

	const std::string& getName() const;

	auto begin() noexcept -> typename std::vector<UIProperty>::iterator;
	auto begin() const noexcept -> typename std::vector<UIProperty>::const_iterator;
	auto end() noexcept -> typename std::vector<UIProperty>::iterator;
	auto end() const noexcept -> typename std::vector<UIProperty>::const_iterator;

private:
	std::string m_name;
	std::vector<UIProperty> m_properties;
};

inline const std::string& UIPropertyGroup::getName() const
{
	return m_name;
}

inline auto UIPropertyGroup::begin() noexcept
-> typename std::vector<UIProperty>::iterator
{
	return m_properties.begin();
}

inline auto UIPropertyGroup::begin() const noexcept
-> typename std::vector<UIProperty>::const_iterator
{
	return m_properties.begin();
}

inline auto UIPropertyGroup::end() noexcept
-> typename std::vector<UIProperty>::iterator
{
	return m_properties.end();
}

inline auto UIPropertyGroup::end() const noexcept
-> typename std::vector<UIProperty>::const_iterator
{
	return m_properties.end();
}

}// end namespace ph::editor
