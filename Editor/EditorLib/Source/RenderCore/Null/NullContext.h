#pragma once

#include "RenderCore/GraphicsContext.h"
#include "RenderCore/Null/NullGHI.h"
#include "RenderCore/Null/NullObjectManager.h"
#include "RenderCore/Null/NullMemoryManager.h"

namespace ph::editor
{

class NullContext : public GraphicsContext
{
public:
	~NullContext() override = default;

	NullGHI& getGHI() override
	{
		return m_ghi;
	}

	NullObjectManager& getObjectManager() override
	{
		return m_objManager;
	}

	NullMemoryManager& getMemoryManager() override
	{
		return m_memManager;
	}

private:
	NullGHI m_ghi;
	NullObjectManager m_objManager;
	NullMemoryManager m_memManager;
};

}// end namespace ph::editor
