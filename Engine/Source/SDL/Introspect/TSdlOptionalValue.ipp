#pragma once

#include "SDL/Introspect/TSdlOptionalValue.h"

#include <utility>

namespace ph
{

template<typename T, typename Owner>
inline TSdlOptionalValue<T, Owner>::TSdlOptionalValue(
	std::string typeName,
	std::string valueName,
	std::optional<T> Owner::* valuePtr)

	: TSdlOptionalValue(typeName, valueName, typeName, valuePtr)
{}

template<typename T, typename Owner>
inline TSdlOptionalValue<T, Owner>::TSdlOptionalValue(
	std::string typeName,
	std::string valueName,
	std::string typeSignature,
	std::optional<T> Owner::* valuePtr)

	: TSdlAbstractValue<T, Owner>(
		std::move(typeName), 
		std::move(valueName),
		std::move(typeSignature))

	, m_valuePtr(valuePtr)
{
	PH_ASSERT(m_valuePtr);

	this->setImportance(EFieldImportance::Optional);
}

template<typename T, typename Owner>
inline auto TSdlOptionalValue<T, Owner>::description(std::string descriptionStr)
-> TSdlOptionalValue&
{
	this->setDescription(std::move(descriptionStr));
	return *this;
}

template<typename T, typename Owner>
inline void TSdlOptionalValue<T, Owner>::setValue(Owner& owner, T value) const
{
	owner.*m_valuePtr = std::move(value);
}

template<typename T, typename Owner>
inline T* TSdlOptionalValue<T, Owner>::getValue(Owner& owner) const
{
	std::optional<T>& optValue = owner.*m_valuePtr;
	return optValue.has_value() ? &optValue.value() : nullptr;
}

template<typename T, typename Owner>
inline const T* TSdlOptionalValue<T, Owner>::getConstValue(const Owner& owner) const
{
	const std::optional<T>& optValue = owner.*m_valuePtr;
	return optValue.has_value() ? &optValue.value() : nullptr;
}

template<typename T, typename Owner>
inline void TSdlOptionalValue<T, Owner>::ownedValueToDefault(Owner& owner) const
{
	owner.*m_valuePtr = std::nullopt;
}

template<typename T, typename Owner>
inline std::optional<T>& TSdlOptionalValue<T, Owner>::valueRef(Owner& owner) const
{
	return owner.*m_valuePtr;
}

}// end namespace ph
