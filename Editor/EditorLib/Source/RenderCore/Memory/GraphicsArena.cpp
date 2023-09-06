#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/ghi_exceptions.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>

namespace ph::editor
{

GraphicsArena::GraphicsArena(GraphicsMemoryManager* manager, EType type)
	: m_manager(manager)
	, m_memoryBlock(nullptr)
	, m_type(type)
{
	if(!m_manager)
	{
		throw GHIBadAllocation{};
	}
}

GraphicsMemoryBlock* GraphicsArena::allocNextBlock()
{
	GraphicsMemoryBlock* newBlock = nullptr;
	switch(m_type)
	{
	case EType::Host:
		// Can only be requested on GHI producer threads.
		PH_ASSERT(!Threads::isOnMainThread());
		PH_ASSERT(!Threads::isOnRenderThread());

		// For GHI producer threads: can be recycled at the end of frame
		newBlock = m_manager->allocHostBlock(0);
		break;

	case EType::RendererHost:
		// Can only be requested on renderer producer threads.
		PH_ASSERT(!Threads::isOnMainThread());
		PH_ASSERT(!Threads::isOnGHIThread());

		// For renderer producer threads: GHI frame may end before renderer frame but can never
		// restart until next renderer frame. Live at least 1 GHI frame so the arena will not be
		// recycled until next renderer frame.
		newBlock = m_manager->allocHostBlock(1);
		break;
	}
	
	if(!newBlock)
	{
		throw GHIBadAllocation{};
	}

	return newBlock;
}

}// end namespace ph::editor
