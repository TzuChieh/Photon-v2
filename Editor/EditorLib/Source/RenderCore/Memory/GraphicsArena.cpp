#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/ghi_exceptions.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>

namespace ph::editor::ghi
{

namespace
{

// For GHI producer threads: can be recycled at the end of frame
inline uint32 single_frame_lifetime()
{
	return 0;
}

// For render producer threads: live for the current render frame and won't be recycled until
// render frame ends (this is 0 render frame).
inline uint32 single_render_frame_lifetime()
{
	return GraphicsMemoryManager::equivalentRenderFrameLifetime(0);
}

}// end anonymous namespace

GraphicsArena::GraphicsArena(GraphicsMemoryManager* manager, EType type)
	: m_manager(manager)
	, m_memoryBlock(nullptr)
	, m_type(type)
{
	if(!m_manager)
	{
		throw BadAllocation{};
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

		newBlock = m_manager->allocHostBlock(single_frame_lifetime());
		break;

	case EType::RenderProducerHost:
		// Can only be requested on renderer producer threads.
		PH_ASSERT(!Threads::isOnMainThread());

		newBlock = m_manager->allocHostBlock(single_render_frame_lifetime());
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}
	
	if(!newBlock)
	{
		throw BadAllocation{};
	}

	return newBlock;
}

GraphicsMemoryBlock* GraphicsArena::allocCustomBlock(std::size_t blockSize, std::size_t blockAlignment)
{
	GraphicsMemoryBlock* newBlock = nullptr;
	switch(m_type)
	{
	case EType::Host:
		// Can only be requested on GHI producer threads.
		PH_ASSERT(!Threads::isOnMainThread());
		PH_ASSERT(!Threads::isOnRenderThread());

		newBlock = m_manager->allocCustomHostBlock(
			single_frame_lifetime(), 
			blockSize, 
			blockAlignment);
		break;

	case EType::RenderProducerHost:
		// Can only be requested on renderer producer threads.
		PH_ASSERT(!Threads::isOnMainThread());

		newBlock = m_manager->allocCustomHostBlock(
			single_render_frame_lifetime(),
			blockSize,
			blockAlignment);
		break;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}
	
	if(!newBlock)
	{
		throw BadAllocation{};
	}

	return newBlock;
}

}// end namespace ph::editor::ghi
