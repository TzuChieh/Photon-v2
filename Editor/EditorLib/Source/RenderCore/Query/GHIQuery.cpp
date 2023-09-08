#include "RenderCore/Query/GHIQuery.h"

namespace ph::editor
{

GHIQuery::~GHIQuery() = default;

bool GHIQuery::isReady() const
{
	if(m_readyFlag.test(std::memory_order_relaxed))
	{
		if(m_readyFlag.test(std::memory_order_acquire))
		{
			return true;
		}
	}

	return false;
}

void GHIQuery::queryDone()
{
	m_readyFlag.test_and_set(std::memory_order_release);
}

}// end namespace ph::editor
