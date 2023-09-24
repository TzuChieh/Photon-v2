#pragma once

#include "EditorCore/Query/TQuery.h"

#include <Utility/Concurrent/TAtomicQuasiQueue.h>
#include <Common/assertion.h>

#include <vector>
#include <utility>
#include <iterator>

#if PH_DEBUG
#include <thread>
#endif

namespace ph::editor
{

template<typename Target>
class TConcurrentQueryManager
{
public:
	/*! @brief Add a query from any thread.
	@brief Thread safe.
	*/
	void addQuery(TQuery<Target> query);

	/*! @brief Process all added queries.
	Need to be called on the same thread consistently.
	*/
	void processQueries(Target& target);

private:
	TAtomicQuasiQueue<TQuery<Target>> m_queries;
	std::vector<TQuery<Target>> m_queryCache;

#if PH_DEBUG
	std::thread::id m_processingThreadId;
#endif
};

template<typename Target>
inline void TConcurrentQueryManager<Target>::addQuery(TQuery<Target> query)
{
	m_queries.enqueue(std::move(query));
}

template<typename Target>
inline void TConcurrentQueryManager<Target>::processQueries(Target& target)
{
#if PH_DEBUG
	if(m_processingThreadId == std::thread::id{})
	{
		m_processingThreadId = std::this_thread::get_id();
	}
	PH_ASSERT(std::this_thread::get_id() == m_processingThreadId);
#endif

	TQuery<Target> query;
	while(m_queries.tryDequeue(&query))
	{
		// The query can simply be skipped if canceled
		if(query.isCanceled())
		{
			continue;
		}

		if(!query.run(target))
		{
			m_queryCache.push_back(std::move(query));
		}
	}

	// Queue queries that were not finished back
	m_queries.enqueueBulk(std::make_move_iterator(m_queryCache.begin()), m_queryCache.size());
	m_queryCache.clear();
}

}// end namespace ph::editor
