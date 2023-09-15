#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/profiling.h>

#include <iterator>

namespace ph::editor
{

GraphicsContext::~GraphicsContext() = default;

void GraphicsContext::load()
{
	PH_PROFILE_SCOPE();

#if PH_DEBUG
	m_ctxThreadId = std::this_thread::get_id();
	PH_ASSERT(isOnContextThread());
#endif

	getGHI().load();

	// Notify GHI load before object manager
	getMemoryManager().onGHILoad();

	getObjectManager().onGHILoad();
}

void GraphicsContext::unload()
{
	PH_PROFILE_SCOPE();

#if PH_DEBUG
	PH_ASSERT(isOnContextThread());
	m_ctxThreadId = std::thread::id{};
#endif

	getObjectManager().onGHIUnload();

	// Notify GHI unload after object manager
	getMemoryManager().onGHIUnload();

	getGHI().unload();
}

void GraphicsContext::beginFrameUpdate(const GHIThreadUpdateContext& updateCtx)
{
	PH_PROFILE_SCOPE();
	PH_ASSERT(isOnContextThread());

	// Begin update before object manager
	getMemoryManager().beginFrameUpdate(updateCtx);

	getObjectManager().beginFrameUpdate(updateCtx);
}

void GraphicsContext::endFrameUpdate(const GHIThreadUpdateContext& updateCtx)
{
	PH_PROFILE_SCOPE();
	PH_ASSERT(isOnContextThread());

	// Processing queries on frame end, this will give some queries higher chance to finish
	// within a single frame
	m_queryManager.processQueries(*this);

	getObjectManager().endFrameUpdate(updateCtx);

	// End update after object manager
	getMemoryManager().endFrameUpdate(updateCtx);
}

}// end namespace ph::editor
