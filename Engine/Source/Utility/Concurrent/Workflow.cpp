#pragma once

#include "Utility/Concurrent/Workflow.h"
#include "Common/assertion.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"
#include "Common/logging.h"

#include <stdexcept>
#include <algorithm>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Workflow, Concurrent);

Workflow::WorkHandle::WorkHandle() :
	WorkHandle(static_cast<std::size_t>(-1), nullptr)
{}

Workflow::WorkHandle::WorkHandle(const std::size_t workId, Workflow* const workflow) :
	m_workId(workId), m_workflow(workflow)
{}

void Workflow::WorkHandle::runsBefore(const WorkHandle succeedingWork)
{
	PH_ASSERT(m_workflow);
	m_workflow->dependsOn(succeedingWork, *this);
}

void Workflow::WorkHandle::runsAfter(const WorkHandle preceedingWork)
{
	PH_ASSERT(m_workflow);
	m_workflow->dependsOn(*this, preceedingWork);
}

Workflow::ManagedWork::ManagedWork(const WorkHandle handle) :
	m_handle(handle)
{
	PH_ASSERT(m_handle);
}

void Workflow::ManagedWork::operator () ()
{
	PH_ASSERT(m_handle.getWorkflow()->m_workDoneFlags);

	const auto  workId            = m_handle.getWorkId();
	const auto& idToDependencyIds = m_handle.getWorkflow()->m_idToDependencyIds;
	auto&       workDoneFlags     = m_handle.getWorkflow()->m_workDoneFlags;

	// Wait for all dependencies to finish first
	const std::vector<std::size_t>& dependencyIds = idToDependencyIds[workId];
	for(std::size_t dependencyWorkId = 0; dependencyWorkId < dependencyIds.size(); ++dependencyWorkId)
	{
		workDoneFlags[dependencyWorkId].wait(false, std::memory_order_acquire);

		PH_ASSERT(workDoneFlags[dependencyWorkId].test(std::memory_order_relaxed));
	}

	// All depending works are done--now we can do this work
	m_handle.getWorkflow()->m_works[workId]();

	// Signify this work is done so works depending on it can unwait/unblock
	const bool hasAlreadyDone = workDoneFlags[workId].test_and_set(std::memory_order_release);
	PH_ASSERT(!hasAlreadyDone);
}

Workflow::Workflow() :
	Workflow(128)
{}

Workflow::Workflow(const std::size_t numExpectedWorks) :
	m_works(), m_idToDependencyIds(), m_workDoneFlags()
{
	m_idToDependencyIds.reserve(std::max<std::size_t>(numExpectedWorks, 128));
}

Workflow::WorkHandle Workflow::addWork(Work work)
{
	if(!work)
	{
		throw std::invalid_argument("provided work is empty");
	}

	m_works.push_back(std::move(work));
	m_idToDependencyIds.push_back(std::vector<std::size_t>());
	PH_ASSERT_EQ(m_works.size(), m_idToDependencyIds.size());

	return WorkHandle(m_works.size() - 1, this);
}

void Workflow::dependsOn(const WorkHandle target, const WorkHandle targetDependency)
{
	ensureValidWorkHandle(target);
	ensureValidWorkHandle(targetDependency);

	PH_ASSERT_LT(target.getWorkId(), m_idToDependencyIds.size());
	m_idToDependencyIds[target.getWorkId()].push_back(targetDependency.getWorkId());
}

void Workflow::runAndWaitAllWorks(FixedSizeThreadPool& workers)
{
	startWorkflow();

	// TODO: topo sort
	for(std::size_t workId = 0; workId < numWorks(); ++workId)
	{
		workers.queueWork(ManagedWork(WorkHandle(workId, this)));
	}

	workers.waitAllWorks();

	finishWorkflow();
}

std::size_t Workflow::numWorks() const
{
	return m_works.size();
}

void Workflow::ensureValidWorkHandle(const WorkHandle work) const
{
	if(!work || work.getWorkflow() != this)
	{
		throw std::invalid_argument("invalid work handle detected");
	}
}

void Workflow::startWorkflow()
{
	// Workflow must had been finished
	PH_ASSERT(!m_workDoneFlags);

	m_workDoneFlags = std::make_unique<std::atomic_flag[]>(numWorks());
}

void Workflow::finishWorkflow()
{
	// Workflow must had been started
	PH_ASSERT(m_workDoneFlags);

	// We do not need the flags anymore, release memory
	m_workDoneFlags = nullptr;
}

std::unique_ptr<std::size_t[]> Workflow::determineDispatchOrderFromTopologicalSort() const
{
	// Every resource has an adjacency list storing the indices to the depending resources.
	// E.g., if resource "i" is the "j"-th resource depending on resource "k" (as "i" depends
	// on "k"), the relation will be represented as "adjacencyLists[k][j] = i"

	std::vector<int> dependentCounts(numResources, 0);

	PH_LOG(Workflow, "building dispatch order from DAG for {} works", numWorks());

	// Builds the DAG (fill in adjacency lists)
	{
		std::vector<const ISdlResource*> tmpReferencedResources;
		std::size_t maxRefCount = 0;

		for(std::size_t resIdx = 0; resIdx < m_resourceInfos.size(); ++resIdx)
		{
			const ResourceInfo& resInfo = m_resourceInfos[resIdx];
			PH_ASSERT(resInfo.resource);

			const SdlClass* const sdlClass = resInfo.resource->getDynamicSdlClass();
			PH_ASSERT(sdlClass);

			tmpReferencedResources.clear();
			sdlClass->associatedResources(*resInfo.resource, tmpReferencedResources);

			maxRefCount = std::max(tmpReferencedResources.size(), maxRefCount);

			for(std::size_t refIdx = 0; refIdx < tmpReferencedResources.size(); ++refIdx)
			{
				const ISdlResource* const referencedRes = tmpReferencedResources[refIdx];
				PH_ASSERT(referencedRes);

				if(referencedRes == resInfo.resource)
				{
					PH_LOG_WARNING(SdlReferenceResolver,
						"resource {} referenced itself, ignoring the reference", resInfo.name);
					continue;
				}

				const auto optReferencedResIdx = getResourceInfoIdx(referencedRes);
				if(!optReferencedResIdx)
				{
					PH_LOG_WARNING(SdlReferenceResolver,
						"resource {} referenced a resource that is not tracked by the analyzed SceneDescription, ignoring the reference",
						resInfo.name);
					continue;
				}

				// Referenced resource index and current resource index must be different
				// (no self referencing)
				PH_ASSERT_NE(resIdx, *optReferencedResIdx);

				// Record resource <resIdx> depends on resource <*optReferencedResIdx>
				adjacencyLists[*optReferencedResIdx].push_back(resIdx);
				dependentCounts[resIdx]++;
			}
		}

		PH_LOG(SdlReferenceResolver, "DAG building done, max references/degree = {}", maxRefCount);
	}// end DAG building

	// Start topological sorting by finding resources without any dependency
	std::queue<std::size_t> independentResIndices;
	for(std::size_t resIdx = 0; resIdx < dependentCounts.size(); ++resIdx)
	{
		if(dependentCounts[resIdx] == 0)
		{
			independentResIndices.push(resIdx);
		}
	}

	PH_LOG(SdlReferenceResolver, "{} resources are already independent", independentResIndices.size());

	// Main topological sorting that produces a valid resource dispatch order
	m_queuedResources = std::queue<const ISdlResource*>();
	while(!independentResIndices.empty())
	{
		const std::size_t independentResIdx = independentResIndices.front();
		independentResIndices.pop();

		m_queuedResources.push(m_resourceInfos[independentResIdx].resource);

		// Mark the resource as dispatched
		PH_ASSERT_EQ(dependentCounts[independentResIdx], 0);
		dependentCounts[independentResIdx] = -1;

		// Update dependent counts of resources depending on the dispatched resource
		// (remove dependency "edge")
		const std::vector<std::size_t>& dependingResIndices = adjacencyLists[independentResIdx];
		for(std::size_t idx = 0; idx < dependingResIndices.size(); ++idx)
		{
			const std::size_t dependingResIdx = dependingResIndices[idx];

			PH_ASSERT_GT(dependentCounts[dependingResIdx], 0);
			dependentCounts[dependingResIdx]--;

			// If the depending resource is now independent, queue it for dispatchment
			if(dependentCounts[dependingResIdx] == 0)
			{
				independentResIndices.push(dependingResIdx);
			}
		}
	}

	// Now a dispatch order that satisfies the dependencies between all resources
	// is established. We check each resource's dependent count to see if there is
	// any error.

	if(m_queuedResources.size() != numResources)
	{
		PH_LOG_WARNING(SdlReferenceResolver, "queued {} resources while there are {} in total", 
			m_queuedResources.size(), numResources);
	}

	for(std::size_t resIdx = 0; resIdx < dependentCounts.size(); ++resIdx)
	{
		const auto dependentCount = dependentCounts[resIdx];
		PH_ASSERT_NE(dependentCount, 0);

		if(dependentCount != -1)
		{
			const ResourceInfo& resInfo = m_resourceInfos[resIdx];
			PH_LOG_WARNING(SdlReferenceResolver, 
				"possible cyclic references detected: resource {} has {} references that cannot be resolved",
				resInfo.name, dependentCount);
		}
	}
}

}// end namespace ph
