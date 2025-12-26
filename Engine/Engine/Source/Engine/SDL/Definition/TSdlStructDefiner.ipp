#pragma once

#include "Engine/SDL/Definition/TSdlStructDefiner.h"

#include <utility>

namespace ph
{

template<typename Def>
inline TSdlStructDefiner<Def>
::TSdlStructDefiner(Def& def)
	: m_def(def)
{}

template<typename Def>
inline auto TSdlStructDefiner<Def>
::typeName(std::string nameStr)
-> TSdlStructDefiner&
{
	m_def.typeName(std::move(nameStr));
	return *this;
}

template<typename Def>
inline auto TSdlStructDefiner<Def>
::description(std::string desc)
-> TSdlStructDefiner&
{
	m_def.description(std::move(desc));
	return *this;
}

template<typename Def>
template<typename FieldType>
inline auto TSdlStructDefiner<Def>
::addField(FieldType&& field)
-> TSdlStructDefiner&
{
	m_def.addField(std::forward<FieldType>(field));
	return *this;
}

template<typename Def>
template<typename OwnerType, typename StructObjType>
inline auto TSdlStructDefiner<Def>
::addStruct(StructObjType OwnerType::* structObjPtr)
-> TSdlStructDefiner&
{
	m_def.addStruct(structObjPtr);
	return *this;
}

}// end namespace ph
