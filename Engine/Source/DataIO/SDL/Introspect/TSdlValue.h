#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"

#include <utility>

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

	bool loadFromSdl(
		Owner&             owner,
		const std::string& sdl,
		std::string&       out_loaderMessage) override = 0;

	void convertToSdl(
		Owner&       owner,
		std::string* out_sdl,
		std::string& out_converterMessage) const override = 0;

	TSdlValue& defaultTo(T defaultValue);
	TSdlValue& withImportance(EFieldImportance importance);

	void setValue(Owner& owner, T value);
	void setValueToDefault(Owner& owner);

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
	m_defaultValue(0)
{
	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline TSdlValue<T, Owner>& TSdlValue<T, Owner>::defaultTo(T defaultValue)
{
	m_defaultValue = std::move(defaultValue);
}

template<typename T, typename Owner>
inline TSdlValue<T, Owner>& TSdlValue<T, Owner>::withImportance(const EFieldImportance importance)
{
	withImportance(importance);

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
	owner.*m_valuePtr = m_defaultValue;
}

}// end namespace ph
