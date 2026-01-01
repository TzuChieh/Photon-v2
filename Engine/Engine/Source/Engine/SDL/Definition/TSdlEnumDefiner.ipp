#pragma once

#include "Engine/SDL/Definition/TSdlEnumDefiner.h"

#include <utility>

namespace ph
{

template<typename Def>
inline TSdlEnumDefiner<Def>
::TSdlEnumDefiner(Def& def)
	: m_def(def)
{}

template<typename Def>
inline auto TSdlEnumDefiner<Def>
::name(std::string nameStr)
-> TSdlEnumDefiner&
{
	m_def.name(std::move(nameStr));
	return *this;
}

template<typename Def>
inline auto TSdlEnumDefiner<Def>
::description(std::string desc)
-> TSdlEnumDefiner&
{
	m_def.description(std::move(desc));
	return *this;
}

template<typename Def>
template<typename EnumType>
inline auto TSdlEnumDefiner<Def>
::addEntry(
	EnumType         enumValue,
	std::string_view valueName,
	std::string      description)
-> TSdlEnumDefiner&
{
	m_def.addEntry(enumValue, valueName, std::move(description));
	return *this;
}
}// end namespace ph
