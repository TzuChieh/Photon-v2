#pragma once

#include "RenderCore/GraphicsContext.h"
#include "RenderCore/Null/NullGHI.h"
#include "RenderCore/Null/NullObjectManager.h"

namespace ph::editor
{

class NullContext : public GraphicsContext
{
public:
	inline ~NullContext() override = default;

	inline NullGHI& getGHI() override
	{
		return m_ghi;
	}

	inline NullObjectManager& getObjectManager() override
	{
		return m_objManager;
	}

private:
	NullGHI m_ghi;
	NullObjectManager m_objManager;
};

}// end namespace ph::editor
