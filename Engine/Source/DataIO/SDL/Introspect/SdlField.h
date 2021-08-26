#pragma once

#include "Common/assertion.h"
#include "Common/logging.h"
#include "DataIO/SDL/SdlResourceId.h"

#include <string>
#include <utility>

namespace ph { class ISdlResource; }

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlField, SDL);

class SdlField
{
public:
	SdlField(std::string typeName, std::string fieldName);
	virtual ~SdlField() = default;

	/*! @brief Get the SDL resource ID of the resource bound to this field.
	@param ownerResource Owner instance of the field. May be nullptr if the field
	is unowned.
	@return A SDL resource ID. May be EMPTY_SDL_RESOURCE_ID if the field is not
	owned by @p ownerResource, or the field is a SDL value type.
	*/
	virtual SdlResourceId retrieveResourceId(const ISdlResource* ownerResource) const = 0;

	std::string genPrettyName() const;
	const std::string& getTypeName() const;
	const std::string& getFieldName() const;
	const std::string& getDescription() const;
	bool isFallbackEnabled() const;

protected:
	SdlField& setEnableFallback(bool isFallbackEnabled);
	SdlField& setDescription(std::string descriptionStr);

private:
	std::string m_typeName;
	std::string m_fieldName;
	std::string m_description;
	bool        m_isFallbackEnabled;
};

// In-header Implementation:

inline SdlField::SdlField(std::string typeName, std::string fieldName) : 
	m_typeName         (std::move(typeName)), 
	m_fieldName        (std::move(fieldName)),
	m_description      (),
	m_isFallbackEnabled(true)
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

inline const std::string& SdlField::getDescription() const
{
	return m_description;
}

inline bool SdlField::isFallbackEnabled() const
{
	return m_isFallbackEnabled;
}

inline SdlField& SdlField::setDescription(std::string descriptionStr)
{
	m_description = std::move(descriptionStr);

	return *this;
}

inline SdlField& SdlField::setEnableFallback(const bool isFallbackEnabled)
{
	m_isFallbackEnabled = isFallbackEnabled;

	return *this;
}

}// end namespace ph
