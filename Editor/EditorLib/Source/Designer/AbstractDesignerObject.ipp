#pragma once

#include "Designer/AbstractDesignerObject.h"

#include <Common/assertion.h>

namespace ph::editor
{

inline auto AbstractDesignerObject::getState() const
-> const TEnumFlags<EObjectState>&
{
	return m_state;
}

inline auto AbstractDesignerObject::state()
-> TEnumFlags<EObjectState>&
{
	return m_state;
}

inline uint64 AbstractDesignerObject::getSceneStorageIndex() const
{
	return m_sceneStorageIndex;
}

inline void AbstractDesignerObject::setSceneStorageIndex(const uint64 storageIndex)
{
	PH_ASSERT_NE(storageIndex, static_cast<uint64>(-1));
	m_sceneStorageIndex = storageIndex;
}

}// end namespace ph::editor
