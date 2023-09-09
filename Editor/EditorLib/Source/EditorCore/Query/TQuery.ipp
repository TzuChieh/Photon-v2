#pragma once

#include "EditorCore/Query/TQuery.h"
#include "EditorCore/Query/TQueryPerformer.h"

#include <Common/logging.h>

namespace ph::editor
{

template<typename Target>
inline TQuery<Target>::TQuery(std::shared_ptr<Performer> performer, EQuery mode)
	: m_performer(std::move(performer))
	, m_numRetries(0)
	, m_mode(mode)
{
	PH_ASSERT(m_performer);
}

template<typename Target>
inline TQuery<Target>::~TQuery() = default;

template<typename Target>
inline bool TQuery<Target>::run(Target& target)
{
	PH_ASSERT(m_performer);

	// A limit that will be reached in 10 minutes, assuming 60 FPS and one try per frame
	constexpr uint32 maxRetries = 60 * 60 * 10;

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
			PH_DEFAULT_LOG_WARNING(
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

}// end namespace ph::editor
