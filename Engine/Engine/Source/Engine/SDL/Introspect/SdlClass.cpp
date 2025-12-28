#include "Engine/SDL/Introspect/SdlClass.h"
#include "Engine/SDL/sdl_helpers.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

SdlClass::SdlClass()
	: m_category(ESdlTypeCategory::Unspecified)
	, m_typeName()
	, m_docName()
	, m_description()
	, m_base(nullptr)
	, m_isBlueprint(false)
	, m_allowCreateFromClass(true)
{}

std::string SdlClass::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

std::string SdlClass::genCategoryName() const
{
	return std::string(sdl::category_to_string(getCategory()));
}

SdlClass& SdlClass::setTypeInfo(ESdlTypeCategory category, std::string typeName)
{
	m_category = category;
	m_typeName = std::move(typeName);

	if(getDocName().empty())
	{
		setDocName(m_typeName);
	}

	PH_ASSERT(!m_typeName.empty());
	PH_ASSERT_MSG(m_category != ESdlTypeCategory::Unspecified,
		"unspecified SDL resource category detected in " + genPrettyName() + "; "
		"consult documentation of ISdlResource and see if the SDL resource is "
		"properly implemented");

	return *this;
}

SdlClass& SdlClass::setDescription(std::string description)
{
	m_description = std::move(description);
	return *this;
}

SdlClass& SdlClass::setDocName(std::string docName)
{
	m_docName = std::move(docName);
	return *this;
}

SdlClass& SdlClass::setIsBlueprint(const bool isBlueprint)
{
	m_isBlueprint = isBlueprint;
	return *this;
}

SdlClass& SdlClass::setAllowCreateFromClass(const bool allowCreateFromClass)
{
	m_allowCreateFromClass = allowCreateFromClass;
	return *this;
}

}// end namespace ph
