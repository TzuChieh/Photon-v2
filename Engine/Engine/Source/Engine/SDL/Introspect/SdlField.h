#pragma once

#include "Engine/SDL/sdl_fwd.h"
#include "Engine/SDL/Introspect/EFieldImportance.h"
#include "Engine/SDL/Introspect/EFieldOption.h"

#include <Common/logging.h>

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

	SdlField(
		std::string typeName, 
		std::string fieldName,
		std::string typeSignature);

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
	std::string_view getTypeSignature() const;

	EFieldImportance getImportance() const;
	FieldOptions getOptions() const;

protected:
	SdlField& setDescription(std::string descriptionStr);

	/*! @brief Sets the importance of the field.
	Different importance affect the underlying error reporting policy used when loading
	and saving the field, e.g., whether warnings are emitted, exceptions are thrown.
	*/
	SdlField& setImportance(EFieldImportance importance);

	SdlField& setOptions(FieldOptions options);

private:
	std::string m_typeName;
	std::string m_fieldName;
	std::string m_description;
	std::string m_typeSignature;
	EFieldImportance m_importance;
	FieldOptions m_options;
};

// In-header Implementation:

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

inline std::string_view SdlField::getTypeSignature() const
{
	return m_typeSignature;
}

inline EFieldImportance SdlField::getImportance() const
{
	return m_importance;
}

inline FieldOptions SdlField::getOptions() const
{
	return m_options;
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

inline SdlField& SdlField::setOptions(FieldOptions options)
{
	m_options = options;

	return *this;
}

}// end namespace ph
