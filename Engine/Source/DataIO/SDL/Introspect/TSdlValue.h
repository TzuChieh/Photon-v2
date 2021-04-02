#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"

#include <utility>
#include <string_view>

namespace ph
{

template<typename T, typename Owner>
class TSdlValue : public TOwnedSdlField<Owner>
{
public:
	TSdlValue(
		std::string typeName, 
		std::string valueName, 
		T Owner::*  valuePtr);

	void setValueToDefault(Owner& owner) override;
	std::string valueToString(Owner& owner) const override;

	void loadFromSdl(
		Owner&             owner,
		const std::string& sdlValue,
		SdlInputContext&   ctx) override = 0;

	void convertToSdl(
		Owner&       owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override = 0;

	TSdlValue& defaultTo(T defaultValue);
	TSdlValue& withImportance(EFieldImportance importance);
	TSdlValue& description(std::string description);

	void setValue(Owner& owner, T value);

	const T& getDefaultValue() const;

protected:
	bool standardFailedLoadHandling(
		Owner&           owner,
		std::string_view failReason,
		std::string&     out_message);

private:
	T Owner::* m_valuePtr;
	T          m_defaultValue;
};

// In-header Implementations:

template<typename T, typename Owner>
inline TSdlValue<T, Owner>::TSdlValue(
	std::string      typeName, 
	std::string      valueName, 
	T Owner::* const valuePtr) : 

	TOwnedSdlField<Owner>(std::move(typeName), std::move(valueName)),

	m_valuePtr    (valuePtr),
	m_defaultValue()
{
	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline TSdlValue<T, Owner>& TSdlValue<T, Owner>::defaultTo(T defaultValue)
{
	m_defaultValue = std::move(defaultValue);

	return *this;
}

template<typename T, typename Owner>
inline TSdlValue<T, Owner>& TSdlValue<T, Owner>::withImportance(const EFieldImportance importance)
{
	setImportance(importance);

	return *this;
}

template<typename T, typename Owner>
inline TSdlValue<T, Owner>& TSdlValue<T, Owner>::description(std::string description)
{
	setDescription(std::move(description));

	return *this;
}

template<typename T, typename Owner>
inline void TSdlValue<T, Owner>::setValue(Owner& owner, T value)
{
	owner.*m_valuePtr = std::move(value);
}

template<typename T, typename Owner>
inline void TSdlValue<T, Owner>::setValueToDefault(Owner& owner)
{
	setValue(owner, m_defaultValue);
}

template<typename T, typename Owner>
inline const T& TSdlValue<T, Owner>::getDefaultValue() const
{
	return m_defaultValue;
}

template<typename T, typename Owner>
inline std::string TSdlValue<T, Owner>::valueToString(Owner& owner) const
{
	return std::to_string(owner.*m_valuePtr);
}

template<typename T, typename Owner>
inline bool TSdlValue<T, Owner>::standardFailedLoadHandling(
	Owner&                 owner,
	const std::string_view failReason,
	std::string&           out_message)
{
	out_message += failReason;

	if(getImportance() != EFieldImportance::REQUIRED)
	{
		setValueToDefault(owner);
		out_message += " (default to " + valueToString(owner) + ")";
		return true;
	}
	else
	{
		// For required field, no default is set
		return false;
	}
}

}// end namespace ph
