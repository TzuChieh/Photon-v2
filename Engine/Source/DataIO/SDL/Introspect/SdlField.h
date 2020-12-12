#pragma once

#include "Common/assertion.h"

#include <string>
#include <utility>

namespace ph
{

class SdlField
{
public:
	SdlField(std::string typeName, std::string fieldName);
	virtual ~SdlField() = default;

	const std::string& getTypeName() const;
	const std::string& getFieldName() const;

private:
	std::string m_typeName;
	std::string m_fieldName;
};

// In-header Implementation:

inline SdlField::SdlField(std::string typeName, std::string fieldName) : 
	m_typeName(std::move(typeName)), m_fieldName(std::move(fieldName))
{
	PH_ASSERT(!m_typeName.empty());
	PH_ASSERT(!m_fieldName.empty());
}

inline const std::string& SdlField::getTypeName() const
{
	return m_typeName;
}

inline const std::string& SdlField::getFieldName() const
{
	return m_fieldName;
}

}// end namespace ph
