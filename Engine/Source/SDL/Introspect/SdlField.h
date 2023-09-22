#pragma once

#include "SDL/sdl_fwd.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "SDL/Introspect/EFieldImportance.h"
#include "SDL/Introspect/SdlNativeData.h"

#include <string>
#include <string_view>
#include <utility>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlField, SDL);

class SdlField
{
public:
	SdlField(std::string typeName, std::string fieldName);
	virtual ~SdlField();

	/*! @brief Direct access to the field memory of a SDL instance.
	Note that this field may not necessarily be bound to a SDL resource type (e.g., may bound to
	any function parameter structs). Empty native data info will be returned in cases where this
	field is not owned by (or being part of) the input instance. Obtaining native data requires
	the input instance be valid during this call and any further usages of the returned native data.
	*/
	virtual SdlNativeData nativeData(SdlNonConstInstance instance) const = 0;

	std::string genPrettyName() const;
	std::string_view getTypeName() const;
	std::string_view getFieldName() const;
	std::string_view getDescription() const;

	EFieldImportance getImportance() const;

	/*! @brief Whether the field want to use built-in mechanism to handle I/O problems.
	An example of this is default field value. With fallback enabled, the field may set itself to
	the default value supplied on error (if available). By default, fallback is enabled.
	*/
	bool isFallbackEnabled() const;

protected:
	SdlField& setDescription(std::string descriptionStr);

	/*! @brief Sets the importance of the field.
	Different importance affect the underlying policy used during the import
	and export of the field, e.g., whether warnings are emitted.
	*/
	SdlField& setImportance(EFieldImportance importance);

	SdlField& setEnableFallback(bool isFallbackEnabled);

private:
	std::string m_typeName;
	std::string m_fieldName;
	std::string m_description;
	EFieldImportance m_importance;
	bool m_isFallbackEnabled;
};

// In-header Implementation:

inline SdlField::SdlField(std::string typeName, std::string fieldName)
	: m_typeName(std::move(typeName))
	, m_fieldName(std::move(fieldName))
	, m_description()
	, m_importance(EFieldImportance::NiceToHave)
	, m_isFallbackEnabled(true)
{
	PH_ASSERT(!m_typeName.empty());
	PH_ASSERT(!m_fieldName.empty());
}

inline std::string_view SdlField::getTypeName() const
{
	return m_typeName;
}

inline std::string_view SdlField::getFieldName() const
{
	return m_fieldName;
}

inline std::string_view SdlField::getDescription() const
{
	return m_description;
}

inline EFieldImportance SdlField::getImportance() const
{
	return m_importance;
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

inline SdlField& SdlField::setImportance(EFieldImportance importance)
{
	m_importance = importance;

	return *this;
}

inline SdlField& SdlField::setEnableFallback(bool isFallbackEnabled)
{
	m_isFallbackEnabled = isFallbackEnabled;

	return *this;
}

}// end namespace ph
