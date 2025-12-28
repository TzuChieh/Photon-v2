#pragma once

#include "Engine/SDL/Definition/TSdlClassDefiner.h"

#include <utility>

namespace ph
{

template<typename Def>
inline TSdlClassDefiner<Def>
::TSdlClassDefiner(Def& def)
	: m_def(def)
{}

template<typename Def>
inline auto TSdlClassDefiner<Def>
::typeName(std::string nameStr)
-> TSdlClassDefiner&
{
	m_def.typeName(std::move(nameStr));
	return *this;
}

template<typename Def>
inline auto TSdlClassDefiner<Def>
::docName(std::string nameStr)
-> TSdlClassDefiner&
{
	m_def.docName(std::move(nameStr));
	return *this;
}

template<typename Def>
inline auto TSdlClassDefiner<Def>
::description(std::string desc)
-> TSdlClassDefiner&
{
	m_def.description(std::move(desc));
	return *this;
}

template<typename Def>
template<typename ClassType>
inline auto TSdlClassDefiner<Def>
::baseOn()
-> TSdlClassDefiner&
{
	m_def.template baseOn<ClassType>();
	return *this;
}

template<typename Def>
template<typename FieldType>
inline auto TSdlClassDefiner<Def>
::addField(FieldType&& field)
-> TSdlClassDefiner&
{
	m_def.addField(std::forward<FieldType>(field));
	return *this;
}

template<typename Def>
template<typename OwnerType, typename StructObjType>
inline auto TSdlClassDefiner<Def>
::addStruct(StructObjType OwnerType::* structObjPtr)
-> TSdlClassDefiner&
{
	m_def.addStruct(structObjPtr);
	return *this;
}

template<typename Def>
template<typename FunctionType>
inline auto TSdlClassDefiner<Def>
::addFunction()
-> TSdlClassDefiner&
{
	m_def.template addFunction<FunctionType>();
	return *this;
}

template<typename Def>
inline auto TSdlClassDefiner<Def>
::allowCreateFromClass(bool isAllowed)
-> TSdlClassDefiner&
{
	m_def.allowCreateFromClass(isAllowed);
	return *this;
}

}// end namespace ph
