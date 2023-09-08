#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"
#include "RenderCore/GraphicsObjectManager.h"
#include "RenderCore/GraphicsMemoryManager.h"
#include "EditorCore/Thread/Threads.h"

#include <Common/assertion.h>

#include <iterator>

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

	// Processing queries on frame end, this will give some queries higher chance to finish
	// within a single frame
	processQueries();

	getObjectManager().endFrameUpdate(updateCtx);

	// End update after object manager
	getMemoryManager().endFrameUpdate(updateCtx);
}

void GraphicsContext::processQueries()
{
	PH_ASSERT(Threads::isOnGHIThread());

	GraphicsQuery query;
	while(m_queries.tryDequeue(&query))
	{
		if(!query.run(*this))
		{
			m_queryCache.push_back(std::move(query));
		}
	}

	// Queue queries that were not finished back
	m_queries.enqueueBulk(std::make_move_iterator(m_queryCache.begin()), m_queryCache.size());
	m_queryCache.clear();
}

}// end namespace ph::editor
