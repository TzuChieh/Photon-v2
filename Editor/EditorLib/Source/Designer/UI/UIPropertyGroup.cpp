#include "Designer/UI/UIPropertyGroup.h"

#include <utility>

namespace ph::editor
{

UIPropertyGroup::UIPropertyGroup(std::string groupName)
	: m_name(std::move(groupName))
	, m_properties()
{}

UIPropertyGroup& UIPropertyGroup::addProperty(UIProperty property, const bool shouldPrepend)
{
	if(shouldPrepend)
	{
		m_properties.insert(m_properties.begin(), std::move(property));
	}
	else
	{
		m_properties.push_back(std::move(property));
	}
	
	return *this;
}

UIPropertyGroup& UIPropertyGroup::addProperties(const UIPropertyGroup& properties, const bool shouldPrepend)
{
	if(shouldPrepend)
	{
		m_properties.insert(
			m_properties.begin(),
			properties.m_properties.begin(),
			properties.m_properties.end());
	}
	else
	{
		m_properties.insert(
			m_properties.end(), 
			properties.m_properties.begin(), 
			properties.m_properties.end());
	}

	return *this;
}

UIProperty* UIPropertyGroup::findProperty(std::string_view fieldName)
{
	for(UIProperty& prop : m_properties)
	{
		if(prop.getFieldName() == fieldName)
		{
			return &prop;
		}
	}
	return nullptr;
}

}// end namespace ph::editor
