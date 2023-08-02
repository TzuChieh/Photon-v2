#include "Designer/UI/UIPropertyLayout.h"
#include "Designer/UI/UIProperty.h"

#include <SDL/Introspect/ISdlInstantiable.h>
#include <SDL/Introspect/SdlClass.h>
#include <SDL/Introspect/SdlField.h>

#include <cstddef>
#include <utility>

namespace ph::editor
{

namespace
{

std::vector<const SdlField*> get_all_fields(const ISdlInstantiable* instantiable)
{
	if(!instantiable)
	{
		return {};
	}

	std::vector<const SdlField*> fields;
	for(std::size_t fieldIdx = 0; fieldIdx < instantiable->numFields(); ++fieldIdx)
	{
		fields.push_back(instantiable->getField(fieldIdx));
	}
	return fields;
}

std::vector<const SdlField*> get_all_fields(const SdlClass* clazz)
{
	std::vector<const SdlField*> fields;

	// Gather fields from most derived to base
	const SdlClass* currentClass = clazz;
	while(currentClass)
	{
		for(std::size_t fieldIdx = 0; fieldIdx < currentClass->numFields(); ++fieldIdx)
		{
			fields.push_back(currentClass->getField(fieldIdx));
		}

		currentClass = currentClass->getBase();
	}

	return fields;
}

}// end anonymous namespace

UIPropertyLayout UIPropertyLayout::makeLinearListing(SdlNonConstInstance instance, std::string groupName)
{
	UIPropertyLayout layout;
	if(!instance)
	{
		return layout;
	}

	std::vector<const SdlField*> fields;
	if(instance.isClass())
	{
		fields = get_all_fields(instance.getClass());
	}
	else
	{
		fields = get_all_fields(instance.getInstantiable());
	}

	UIPropertyGroup group(groupName);
	for(const SdlField* field : fields)
	{
		group.addProperty(UIProperty(instance, field));
	}

	layout.addGroup(group);
	return layout;
}

UIPropertyLayout& UIPropertyLayout::addGroup(UIPropertyGroup group, const bool shouldPrepend)
{
	if(shouldPrepend)
	{
		m_groups.insert(m_groups.begin(), std::move(group));
	}
	else
	{
		m_groups.push_back(std::move(group));
	}

	return *this;
}

UIPropertyLayout& UIPropertyLayout::addGroups(const UIPropertyLayout& groups, const bool shouldPrepend)
{
	if(shouldPrepend)
	{
		m_groups.insert(
			m_groups.begin(),
			groups.m_groups.begin(),
			groups.m_groups.end());
	}
	else
	{
		m_groups.insert(
			m_groups.end(),
			groups.m_groups.begin(),
			groups.m_groups.end());
	}

	return *this;
}

UIPropertyGroup* UIPropertyLayout::getGroup(std::string_view groupName)
{
	for(UIPropertyGroup& group : m_groups)
	{
		if(group.getName() == groupName)
		{
			return &group;
		}
	}
	return nullptr;
}

UIPropertyGroup& UIPropertyLayout::getOrCreateGroup(std::string_view groupName, const bool shouldPrepend)
{
	UIPropertyGroup* existingGroup = getGroup(groupName);
	if(existingGroup)
	{
		return *existingGroup;
	}
	else
	{
		if(shouldPrepend)
		{
			m_groups.insert(m_groups.begin(), UIPropertyGroup(std::string(groupName)));
			return m_groups.front();
		}
		else
		{
			m_groups.push_back(UIPropertyGroup(std::string(groupName)));
			return m_groups.back();
		}
	}
}

}// end namespace ph::editor
