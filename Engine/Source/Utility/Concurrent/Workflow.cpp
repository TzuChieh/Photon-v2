#pragma once

#include "Utility/Concurrent/Workflow.h"
#include "Common/assertion.h"

#include <stdexcept>
#include <format>
#include <limits>
#include <utility>
#include <algorithm>

namespace ph
{

Workflow::WorkHandle::WorkHandle(const std::size_t workId, Workflow* const workflow) :
	m_workId(workId), m_workflow(workflow)
{}

void Workflow::WorkHandle::runsBefore(WorkHandle succeedingWork)
{

}

void Workflow::WorkHandle::runsAfter(WorkHandle preceedingWork)
{

}

void Workflow::WorkHandle::runsBefore(std::initializer_list<WorkHandle> succeedingWorks)
{
	for(WorkHandle work : succeedingWorks)
	{
		runsBefore(work);
	}
}

void Workflow::WorkHandle::runsAfter(std::initializer_list<WorkHandle> preceedingWorks)
{
	for(WorkHandle work : preceedingWorks)
	{
		runsAfter(work);
	}
}

Workflow::Workflow() :
	Workflow(128)
{}

Workflow::Workflow(const std::size_t numExpectedWorks) :
	m_works(), m_idToDependentIds()
{
	m_idToDependentIds.reserve(std::max<std::size_t>(numExpectedWorks, 128));
}

Workflow::WorkHandle Workflow::addWork(Work work)
{
	if(!work)
	{
		throw std::invalid_argument("provided work is empty");
	}

	m_works.push_back(std::move(work));
	m_idToDependentIds.push_back(std::vector<std::size_t>());
	PH_ASSERT_EQ(m_works.size(), m_idToDependentIds.size());

	return WorkHandle(m_works.size() - 1, this);
}

Workflow::WorkHandle Workflow::addWorks(std::initializer_list<Work> works)
{
	// TODO
}

std::size_t Workflow::numWorks() const
{
	return m_works.size();
}

template<typename Index, bool IS_DIRECTED>
inline void TAdjacencyListGraph<Index, IS_DIRECTED>::addVertex()
{
	addVertex(1);
}

template<typename Index, bool IS_DIRECTED>
inline void TAdjacencyListGraph<Index, IS_DIRECTED>::addVertices(const std::size_t numAddedVertices)
{
	m_vertexToNeighbors.resize(m_vertexToNeighbors.size() + numAddedVertices);
}

template<typename Index, bool IS_DIRECTED>
inline std::size_t TAdjacencyListGraph<Index, IS_DIRECTED>::numVertices() const
{
	return m_vertexToNeighbors.size();
}

template<typename Index, bool IS_DIRECTED>
inline void TAdjacencyListGraph<Index, IS_DIRECTED>::addEdge(std::size_t fromVertex, std::size_t toVertex)
{
	if constexpr(!IS_DIRECTED)
	{
		// For undirected graph, edge always start with the smaller vertex index, this saves space and time
		if(fromVertex > toVertex)
		{
			std::swap(fromVertex, toVertex);
		}

		PH_ASSERT_LE(fromVertex, toVertex);
	}

	if(toVertex > std::numeric_limits<Index>::max())
	{
		throw std::overflow_error(std::format(
			"second edge vertex index {} cannot be represented (overflow) by Index (max: {})",
			toVertex, std::numeric_limits<Index>::max()));
	}

	PH_ASSERT_LT(fromVertex, m_vertexToNeighbors.size());
	PH_ASSERT_LT(toVertex, m_vertexToNeighbors.size());
	m_vertexToNeighbors[fromVertex].push_back(static_cast<Index>(toVertex));
}

template<typename Index, bool IS_DIRECTED>
inline std::size_t TAdjacencyListGraph<Index, IS_DIRECTED>::numDegrees(const std::size_t vertex) const
{
	PH_ASSERT_LT(vertex, m_vertexToNeighbors.size());
	return m_vertexToNeighbors[vertex].size();
}

template<typename Index, bool IS_DIRECTED>
inline bool TAdjacencyListGraph<Index, IS_DIRECTED>::hasEdge(const std::size_t fromVertex, const std::size_t toVertex) const
{
	PH_ASSERT_LT(vertex, m_vertexToNeighbors.size());
	for()
}

}// end namespace ph
