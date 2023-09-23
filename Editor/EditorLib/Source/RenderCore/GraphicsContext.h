#pragma once

#include "RenderCore/Query/query_basics.h"
#include "EditorCore/Query/TConcurrentQueryManager.h"

#include <Common/assertion.h>

#include <utility>

#if PH_DEBUG
#include <thread>
#endif

namespace ph::editor { class GHIThreadUpdateContext; }

namespace ph::editor::ghi
{

class GHI;
class GraphicsObjectManager;
class GraphicsMemoryManager;

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

	void addQuery(ghi::Query query);

private:
	TConcurrentQueryManager<GraphicsContext> m_queryManager;

#if PH_DEBUG
private:
	bool isOnContextThread() const
	{
		return std::this_thread::get_id() == m_ctxThreadId;
	}

	std::thread::id m_ctxThreadId;
#endif
};

inline void GraphicsContext::addQuery(ghi::Query query)
{
	m_queryManager.addQuery(std::move(query));
}

}// end namespace ph::editor::ghi
