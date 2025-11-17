#pragma once

#include "Engine/SDL/Definition/TSdlFunctionDefiner.h"

#include <utility>

namespace ph
{

template<typename Def>
inline TSdlFunctionDefiner<Def>
::TSdlFunctionDefiner(Def& def)
	: m_def(def)
{}

template<typename Def>
template<typename T>
inline auto TSdlFunctionDefiner<Def>
::addParam(T&& param)
-> TSdlFunctionDefiner&
{
	m_def.addParam(std::forward<T>(param));
	return *this;
}

template<typename Def>
inline auto TSdlFunctionDefiner<Def>
::description(std::string desc)
-> TSdlFunctionDefiner&
{
	m_def.description(std::move(desc));
	return *this;
}

}// end namespace ph
