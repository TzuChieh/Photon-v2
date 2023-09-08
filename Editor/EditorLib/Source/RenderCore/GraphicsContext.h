#pragma once

#include "RenderCore/Query/GraphicsQuery.h"

#include <Utility/Concurrent/TAtomicQuasiQueue.h>

#include <vector>
#include <utility>

namespace ph::editor
{

class GHI;
class GraphicsObjectManager;
class GraphicsMemoryManager;
class GHIThreadUpdateContext;

class GraphicsContext
{
public:
	virtual ~GraphicsContext();

	virtual GHI& getGHI() = 0;
	virtual GraphicsObjectManager& getObjectManager() = 0;
	virtual GraphicsMemoryManager& getMemoryManager() = 0;

	/*! @brief Called by GHI thread to load and initiate GHI.
	*/
	void load();

	/*! @brief Called by GHI thread to unload and cleanup GHI.
	*/
	void unload();

	/*! @brief Called by GHI thread when a frame begins.
	*/
	void beginFrameUpdate(const GHIThreadUpdateContext& updateCtx);

	/*! @brief Called by GHI thread when a frame ends.
	*/
	void endFrameUpdate(const GHIThreadUpdateContext& updateCtx);

	void addQuery(GraphicsQuery query);

private:
	void processQueries();

	TAtomicQuasiQueue<GraphicsQuery> m_queries;
	std::vector<GraphicsQuery> m_queryCache;
};

inline void GraphicsContext::addQuery(GraphicsQuery query)
{
	m_queries.enqueue(std::move(query));
}

}// end namespace ph::editor
