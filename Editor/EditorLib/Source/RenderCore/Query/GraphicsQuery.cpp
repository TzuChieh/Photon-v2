#include "RenderCore/Query/GraphicsQuery.h"
#include "RenderCore/Query/GHIQuery.h"

namespace ph::editor
{

GraphicsQuery::GraphicsQuery(std::shared_ptr<GHIQuery> query, EGHIQuery mode)
	: m_query(std::move(query))
	, m_numRetries(0)
	, m_mode(mode)
{
	PH_ASSERT(m_query);
}

GraphicsQuery::~GraphicsQuery() = default;

bool GraphicsQuery::run(GraphicsContext& ctx)
{
	PH_ASSERT(m_query);

	// A limit that will be reached in 10 minutes, assuming 60 FPS and one try per frame
	constexpr uint32 maxRetries = 60 * 60 * 10;

	bool isDone = m_query->performQuery(ctx);

	// Modify the done flag according to query mode
	switch(m_mode)
	{
	case EGHIQuery::Once:
		isDone = true;
		break;

	case EGHIQuery::AutoRetry:
		if(m_numRetries < maxRetries)
		{
			isDone = true;
		}
		break;
	}

	if(isDone)
	{
		m_query->queryDone();
	}

	++m_numRetries;
	return isDone;
}

}// end namespace ph::editor
