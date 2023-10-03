#pragma once

#include <SDL/TSdlAnyInstance.h>
#include <SDL/sdl_fwd.h>

#include <string>

namespace ph::editor
{

class UIProperty final
{
public:
	UIProperty(SdlNonConstInstance instance, std::string fieldName);
	UIProperty(SdlNonConstInstance instance, const SdlField* field);

	/*! @brief Supply custom display name.
	*/
	UIProperty& withDisplayName(std::string displayName);

	/*! @brief Supply custom help message.
	*/
	UIProperty& withHelpMessage(std::string helpMessage);

	const SdlField* getField() const;
	SdlNativeData getData() const;
	const std::string& getFieldName() const;
	const std::string& getDisplayName() const;
	const std::string& getHelpMessage() const;
	UIProperty& setReadOnly(bool inIsReadOnly);
	bool isReadOnly() const;

private:
	static std::string getHelpMessage(const SdlField* field);

	SdlNonConstInstance m_instance;
	const SdlField* m_field;
	std::string m_fieldName;
	std::string m_displayName;
	std::string m_helpMessage;
	bool m_isReadOnly;
};

inline const SdlField* UIProperty::getField() const
{
	return m_field;
}

inline const std::string& UIProperty::getFieldName() const
{
	return m_fieldName;
}

inline const std::string& UIProperty::getDisplayName() const
{
	return m_displayName;
}

inline const std::string& UIProperty::getHelpMessage() const
{
	return m_helpMessage;
}

inline bool UIProperty::isReadOnly() const
{
	return m_isReadOnly;
}

}// end namespace ph::editor
