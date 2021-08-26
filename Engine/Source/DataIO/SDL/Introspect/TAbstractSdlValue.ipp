#pragma once

#include "DataIO/SDL/Introspect/TAbstractSdlValue.h"

namespace ph
{

template<typename T, typename Owner>
inline TAbstractSdlValue<T, Owner>::TAbstractSdlValue(
	std::string typeName, 
	std::string valueName) : 

	TOwnedSdlField<Owner>(std::move(typeName), std::move(valueName))
{}

template<typename T, typename Owner>
inline std::string TAbstractSdlValue<T, Owner>::valueToString(const Owner& owner) const
{
	const T* const valuePtr = getValue(owner);
	return valuePtr ? valueAsString(*valuePtr) : "(empty)";
}

template<typename T, typename Owner>
inline const ISdlResource* TAbstractSdlValue<T, Owner>::associatedResource(const Owner& owner) const
{
	// Returns nullptr always--SDL value is not a reference type and thus cannot have associated resource
	return nullptr;
}

}// end namespace ph
