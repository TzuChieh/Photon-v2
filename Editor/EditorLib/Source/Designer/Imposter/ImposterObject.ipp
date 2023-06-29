#pragma once

#include "Designer/Imposter/ImposterObject.h"

namespace ph::editor
{

inline ISdlResource* ImposterObject::getTargetResource() const
{
	return m_targetResource.get();
}

inline const std::string& ImposterObject::getTargetName() const
{
	return m_targetName;
}

}// end namespace ph::editor
