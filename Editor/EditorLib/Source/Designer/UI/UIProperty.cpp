#include "Designer/UI/UIProperty.h"

#include <SDL/sdl_helpers.h>
#include <SDL/Introspect/ISdlInstantiable.h>
#include <SDL/Introspect/SdlField.h>

#include <utility>

namespace ph::editor
{

UIProperty::UIProperty(SdlNonConstInstance instance, std::string fieldName)
	: m_instance(instance)
	, m_field(nullptr)
	, m_fieldName(fieldName)
	, m_displayName(sdl::name_to_title_case(m_fieldName))
	, m_helpMessage()
{
	const ISdlInstantiable* instantiable = instance.getInstantiable();
	if(instantiable)
	{
		for(std::size_t fieldIdx = 0; fieldIdx < instantiable->numFields(); ++fieldIdx)
		{
			const SdlField* field = instantiable->getField(fieldIdx);
			if(field->getFieldName() == fieldName)
			{
				m_field = field;
				m_helpMessage = getHelpMessage(field);
				break;
			}
		}
	}
}

UIProperty::UIProperty(SdlNonConstInstance instance, const SdlField* field)
	: m_instance(instance)
	, m_field(field)
	, m_fieldName(field ? field->getFieldName() : "")
	, m_displayName(sdl::name_to_title_case(m_fieldName))
	, m_helpMessage(getHelpMessage(field))
{}

UIProperty& UIProperty::withDisplayName(std::string displayName)
{
	m_displayName = std::move(displayName);

	return *this;
}

UIProperty& UIProperty::withHelpMessage(std::string helpMessage)
{
	m_helpMessage = std::move(helpMessage);

	return *this;
}

SdlNativeData UIProperty::getData() const
{
	if(!m_field)
	{
		// Cannot get native data
		return {};
	}

	return m_field->nativeData(m_instance);
}

std::string UIProperty::getHelpMessage(const SdlField* field)
{
	if(!field)
	{
		return {};
	}

	return std::string(field->getDescription());
}

}// end namespace ph::editor
