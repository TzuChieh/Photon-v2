#pragma once

#include "DataIO/SDL/Introspect/TSdlValue.h"

#include <utility>

namespace ph
{

template<typename T, typename Owner>
inline TSdlValue<T, Owner>::TSdlValue(
	std::string      typeName, 
	std::string      valueName, 
	T Owner::* const valuePtr) : 

	TAbstractSdlValue<T, Owner>(std::move(typeName), std::move(valueName)),

	m_valuePtr    (valuePtr),
	m_defaultValue()
{
	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlValue<T, Owner>::setValue(Owner& owner, T value) const
{
	owner.*m_valuePtr = std::move(value);
}

template<typename T, typename Owner>
inline const T* TSdlValue<T, Owner>::getValue(const Owner& owner) const
{
	return &(owner.*m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlValue<T, Owner>::setValueToDefault(Owner& owner) const
{
	setValue(owner, m_defaultValue);
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::defaultTo(T defaultValue)
-> TSdlValue&
{
	m_defaultValue = std::move(defaultValue);
	return *this;
}

template<typename T, typename Owner>
inline const T& TSdlValue<T, Owner>::defaultValue() const
{
	return m_defaultValue;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::withImportance(const EFieldImportance importance)
-> TSdlValue&
{
	setImportance(importance);
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::description(std::string descriptionStr)
-> TSdlValue&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::optional()
-> TSdlValue&
{
	return withImportance(EFieldImportance::OPTIONAL);
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::niceToHave()
-> TSdlValue&
{
	return withImportance(EFieldImportance::NICE_TO_HAVE);
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::required()
-> TSdlValue&
{
	return withImportance(EFieldImportance::REQUIRED);
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::enableFallback(const bool isFallbackEnabled)
-> TSdlValue&
{
	setEnableFallback(isFallbackEnabled);
	return *this;
}

}// end namespace ph
