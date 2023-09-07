#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>

namespace ph::editor
{

GraphicsContext::~GraphicsContext() = default;

void GraphicsContext::load()
{
	PH_ASSERT(Threads::isOnGHIThread());

	getGHI().load();

	// Notify GHI load before object manager
	getMemoryManager().onGHILoad();

	getObjectManager().onGHILoad();
}

void GraphicsContext::unload()
{
	PH_ASSERT(Threads::isOnGHIThread());

	getObjectManager().onGHIUnload();

	// Notify GHI unload after object manager
	getMemoryManager().onGHIUnload();

	getGHI().unload();
}

void GraphicsContext::beginFrameUpdate(const GHIThreadUpdateContext& updateCtx)
{
	PH_ASSERT(Threads::isOnGHIThread());

	// Begin update before object manager
	getMemoryManager().beginFrameUpdate(updateCtx);

	getObjectManager().beginFrameUpdate(updateCtx);
}

void GraphicsContext::endFrameUpdate(const GHIThreadUpdateContext& updateCtx)
{
	PH_ASSERT(Threads::isOnGHIThread());

	getObjectManager().endFrameUpdate(updateCtx);

	// End update after object manager
	getMemoryManager().endFrameUpdate(updateCtx);
}

}// end namespace ph::editor
