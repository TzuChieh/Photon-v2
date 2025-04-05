#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <queue>

namespace ph
{

//template<typename Item>
//class TSortedNNResult
//{
//public:
//	TSortedNNResult(std::size_t maxResults);
//
//	void addResult(const Item& item);
//	std::size_t numResults() const;
//	const Item& getFurthest() const;
//	std::size_t maxResults() const;
//
//	void clear();
//
//private:
//	std::priority_queue<Item> m_results;
//	std::size_t               m_maxResults;
//};
//
//// In-header Implementations:
//
//template<typename Item>
//inline TSortedNNResult<Item>::TSortedNNResult(const std::size_t maxResults) : 
//	m_results(),
//	m_maxResults(maxResults)
//{
//	PH_ASSERT(maxResults > 0);
//}
//
//template<typename Item>
//inline void TSortedNNResult<Item>::addResult(const Item& item)
//{
//
//}
//
//template<typename Item>
//inline std::size_t TSortedNNResult<Item>::numResults() const
//{
//
//}
//
//template<typename Item>
//inline const Item& TSortedNNResult<Item>::getFurthest() const
//{
//
//}
//
//template<typename Item>
//inline std::size_t TSortedNNResult<Item>::maxResults() const
//{
//
//}
//
//template<typename Item>
//inline void TSortedNNResult<Item>::clear()
//{
//	m_results = std::priority_queue<Item>();
//}

}// end namespace ph
