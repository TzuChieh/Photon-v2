#pragma once

#include "EditorCore/Query/TQuery.h"
#include "EditorCore/Query/TQueryPerformer.h"

#include <Common/logging.h>

namespace ph::editor
{

template<typename Target, typename Performer>
inline TQuery<Target, Performer>::TQuery(std::shared_ptr<Performer> performer, EQuery mode)
	: m_performer(std::move(performer))
	, m_numRetries(0)
	, m_mode(mode)
{
	PH_ASSERT(m_performer);
}

template<typename Target, typename Performer>
template<CDerived<Performer> DerivedPerformer>
inline TQuery<Target, Performer>::TQuery(const TQuery<Target, DerivedPerformer>& derivedQuery)
	: m_performer(derivedQuery.m_performer)
	, m_numRetries(derivedQuery.m_numRetries)
	, m_mode(derivedQuery.m_mode)
{}

template<typename Target, typename Performer>
template<CDerived<Performer> DerivedPerformer>
inline TQuery<Target, Performer>::TQuery(TQuery<Target, DerivedPerformer>&& derivedQuery)
	: m_performer(std::move(derivedQuery.m_performer))
	, m_numRetries(derivedQuery.m_numRetries)
	, m_mode(derivedQuery.m_mode)
{}

template<typename Target, typename Performer>
inline TQuery<Target, Performer>::~TQuery() = default;

template<typename Target, typename Performer>
inline bool TQuery<Target, Performer>::run(Target& target)
{
	PH_ASSERT(m_performer);
	PH_ASSERT(!m_performer->isReady());

	// A limit that will be reached in 1 minute, assuming 60 FPS and one try per frame
	constexpr uint32 maxRetries = 60 * 60;

	bool isDone = m_performer->performQuery(target);

	// Modify the done flag according to query mode
	switch(m_mode)
	{
	case EQuery::Once:
		isDone = true;
		break;

	case EQuery::AutoRetry:
		if(m_numRetries >= maxRetries)
		{
			PH_DEFAULT_LOG(Warning,
				"Detected hanging graphics query ({} retries), canceling",
				m_numRetries);

			isDone = true;
		}
		break;
	}

	if(isDone)
	{
		m_performer->queryDone();
	}

	++m_numRetries;
	return isDone;
}

template<typename Target, typename Performer>
inline void TQuery<Target, Performer>::cancel()
{
	if(m_performer)
	{
		m_performer->cancel();
	}
}

template<typename Target, typename Performer>
inline bool TQuery<Target, Performer>::isCanceled() const
{
	PH_ASSERT(m_performer);
	return m_performer->isCanceled();
}

}// end namespace ph::editor
