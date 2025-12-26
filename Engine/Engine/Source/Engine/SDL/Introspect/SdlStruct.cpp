#include "Engine/SDL/Introspect/SdlStruct.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

SdlStruct::SdlStruct() :
	m_typeName   (),
	m_description()
{}

SdlStruct& SdlStruct::setTypeName(std::string name)
{
	m_typeName = std::move(name);

	return *this;
}

SdlStruct& SdlStruct::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

}// end namespace ph
