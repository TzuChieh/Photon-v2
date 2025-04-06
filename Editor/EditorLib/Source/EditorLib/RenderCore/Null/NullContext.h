#pragma once

#include "EditorLib/RenderCore/GraphicsContext.h"
#include "EditorLib/RenderCore/Null/NullGHI.h"
#include "EditorLib/RenderCore/Null/NullObjectManager.h"
#include "EditorLib/RenderCore/Null/NullMemoryManager.h"

namespace ph::editor::ghi
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

}// end namespace ph::editor::ghi
