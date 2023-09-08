#pragma once

#include <atomic>

namespace ph::editor
{

class GraphicsContext;

class GHIQuery
{
public:
	virtual ~GHIQuery();

	/*!
	@return Whether the query is successful.
	*/
	virtual bool performQuery(GraphicsContext& ctx) = 0;

	/*! @brief Whether the query was finished and the result is ready.
	@return Whether the query result is ready to be used. Memory effects done by `performQuery()`
	on another thread are made visible to the thread that observes `isReady() == true`.
	*/
	bool isReady() const;

private:
	friend class GraphicsQuery;

	/*! @brief Signify the query was finished and mark it as ready.
	Memory effects of `performQuery()` are made visible to whichever thread that sees
	`isReady() == true`. Basically a release-acquire synchronization.
	*/
	void queryDone();

	std::atomic_flag m_readyFlag;
};

}// end namespace ph::editor
