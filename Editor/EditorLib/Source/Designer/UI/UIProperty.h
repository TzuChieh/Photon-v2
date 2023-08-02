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

	UIProperty& withDisplayName(std::string displayName);

	const SdlField* getField() const;
	SdlNativeData getData() const;
	const std::string& getDisplayName() const;

private:
	SdlNonConstInstance m_instance;
	const SdlField* m_field;
	std::string m_fieldName;
	std::string m_displayName;
};

inline const SdlField* UIProperty::getField() const
{
	return m_field;
}

inline const std::string& UIProperty::getDisplayName() const
{
	return m_displayName;
}

}// end namespace ph::editor
