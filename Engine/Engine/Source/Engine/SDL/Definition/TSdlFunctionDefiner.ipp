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
inline auto TSdlFunctionDefiner<Def>
::name(std::string nameStr)
-> TSdlFunctionDefiner&
{
	m_def.name(std::move(nameStr));
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

template<typename Def>
template<typename ParamType>
inline auto TSdlFunctionDefiner<Def>
::addParam(ParamType&& param)
-> TSdlFunctionDefiner&
{
	m_def.addParam(std::forward<ParamType>(param));
	return *this;
}

}// end namespace ph
