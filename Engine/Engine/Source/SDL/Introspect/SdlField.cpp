#include "SDL/Introspect/SdlField.h"
#include "SDL/sdl_helpers.h"

#include <Common/assertion.h>

namespace ph
{

SdlField::SdlField(std::string typeName, std::string fieldName)
	: SdlField(typeName, fieldName, typeName)
{}

SdlField::SdlField(
	std::string typeName,
	std::string fieldName,
	std::string typeSignature)

	: m_typeName(std::move(typeName))
	, m_fieldName(std::move(fieldName))
	, m_description()
	, m_typeSignature(std::move(typeSignature))
	, m_importance(EFieldImportance::NiceToHave)
	, m_isFallbackEnabled(true)
{
	PH_ASSERT(!m_typeName.empty());
	PH_ASSERT(!m_fieldName.empty());
	PH_ASSERT(!m_typeSignature.empty());
}

SdlField::~SdlField() = default;

std::string SdlField::genPrettyName() const
{
	return sdl::gen_pretty_name(this);
}

}// end namespace ph