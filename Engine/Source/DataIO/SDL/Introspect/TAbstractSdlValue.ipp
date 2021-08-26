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
inline SdlResourceId TAbstractSdlValue<T, Owner>::retrieveResourceId(const ISdlResource* const ownerResource) const
{
	// Returns empty ID always--SDL value is not a reference type and thus cannot have resource ID
	return EMPTY_SDL_RESOURCE_ID;
}

}// end namespace ph
