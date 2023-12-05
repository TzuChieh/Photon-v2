#include "SDL/Introspect/SdlStruct.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

SdlStruct::SdlStruct(std::string typeName) :
	m_typeName   (std::move(typeName)),
	m_description()
{
	PH_ASSERT(!m_typeName.empty());
}

SdlStruct& SdlStruct::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

}// end namespace ph
