#pragma once

#include "Designer/Imposter/ImposterObject.h"

namespace ph::editor
{

inline const std::string& ImposterObject::getTargetName() const
{
	return m_targetName;
}

}// end namespace ph::editor
