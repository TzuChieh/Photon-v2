#pragma once

#include "SDL/Introspect/TSdlValue.h"

#include <utility>

namespace ph
{

template<typename T, typename Owner>
inline TSdlValue<T, Owner>::TSdlValue(
	std::string typeName, 
	std::string valueName, 
	T Owner::* valuePtr)

	: TSdlValue(typeName, valueName, typeName, valuePtr)
{}

template<typename T, typename Owner>
inline TSdlValue<T, Owner>::TSdlValue(
	std::string typeName, 
	std::string valueName,
	std::string typeSignature,
	T Owner::* valuePtr)

	: TSdlAbstractValue<T, Owner>(
		std::move(typeName), 
		std::move(valueName),
		std::move(typeSignature))

	, m_valuePtr(valuePtr)
	, m_defaultValue(T{})
{
	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlValue<T, Owner>::setValue(Owner& owner, T value) const
{
	owner.*m_valuePtr = std::move(value);
}

template<typename T, typename Owner>
inline T* TSdlValue<T, Owner>::getValue(Owner& owner) const
{
	return &(owner.*m_valuePtr);
}

template<typename T, typename Owner>
inline const T* TSdlValue<T, Owner>::getConstValue(const Owner& owner) const
{
	return &(owner.*m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlValue<T, Owner>::ownedValueToDefault(Owner& owner) const
{
	if(m_defaultValue.has_value())
	{
		setValue(owner, *m_defaultValue);
	}
}

template<typename T, typename Owner>
inline const T* TSdlValue<T, Owner>::getDefaultValue() const
{
	return m_defaultValue.has_value() ? &(*m_defaultValue) : nullptr;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::defaultTo(T defaultValue)
-> TSdlValue&
{
	m_defaultValue = std::move(defaultValue);
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::noDefault()
-> TSdlValue&
{
	m_defaultValue = std::nullopt;
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::withImportance(const EFieldImportance importance)
-> TSdlValue&
{
	this->setImportance(importance);
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::description(std::string descriptionStr)
-> TSdlValue&
{
	this->setDescription(std::move(descriptionStr));
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::optional()
-> TSdlValue&
{
	return withImportance(EFieldImportance::Optional);
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::niceToHave()
-> TSdlValue&
{
	return withImportance(EFieldImportance::NiceToHave);
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::required()
-> TSdlValue&
{
	return withImportance(EFieldImportance::Required);
}

template<typename T, typename Owner>
inline auto TSdlValue<T, Owner>::enableFallback(const bool isFallbackEnabled)
-> TSdlValue&
{
	this->setEnableFallback(isFallbackEnabled);
	return *this;
}

template<typename T, typename Owner>
inline T& TSdlValue<T, Owner>::valueRef(Owner& owner) const
{
	return owner.*m_valuePtr;
}

}// end namespace ph
