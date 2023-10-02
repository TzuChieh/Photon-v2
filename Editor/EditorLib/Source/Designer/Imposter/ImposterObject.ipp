#pragma once

#include "Designer/Imposter/ImposterObject.h"

namespace ph::editor
{

inline const std::string& ImposterObject::getDescriptionName() const
{
	return m_descName;
}

inline ISdlResource* ImposterObject::getDescriptionResource() const
{
	return m_descResource.get();
}

}// end namespace ph::editor
