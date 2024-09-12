#include "Utility/Concurrent/Workflow.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <stdexcept>
#include <algorithm>
#include <format>

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
	for(const std::size_t dependencyId : idToDependencyIds[workId])
	{
		workDoneFlags[dependencyId].wait(false, std::memory_order_acquire);

		PH_ASSERT(workDoneFlags[dependencyId].test(std::memory_order_relaxed));
	}

	// All depending works are done--now we can do this work
	m_handle.getWorkflow()->m_works[workId]();

	// Signify this work is done so works depending on it will not wait/block
	const bool hasAlreadyDone = workDoneFlags[workId].test_and_set(std::memory_order_release);
	PH_ASSERT(!hasAlreadyDone);

	// Notify all works that are already waiting so they can unwait/unblock
	workDoneFlags[workId].notify_all();
}

Workflow::Workflow() :
	Workflow(0)
{}

Workflow::Workflow(const std::size_t numExpectedWorks) :
	m_works(), m_idToDependencyIds(), m_workDoneFlags()
{
	m_idToDependencyIds.reserve(numExpectedWorks);
	//m_idToDependencyIds.reserve(std::max<std::size_t>(numExpectedWorks, 128));
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

Workflow::WorkHandle Workflow::acquireWork(const std::size_t workIndex)
{
	if(workIndex >= numWorks())
	{
		throw std::out_of_range(std::format(
			"work index {} out of range [0, {})",
			workIndex, numWorks()));
	}

	return WorkHandle(workIndex, this);
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

	{
		const auto workDispatchOrder = determineDispatchOrderFromTopologicalSort();
		PH_ASSERT(workDispatchOrder);

		for(std::size_t order = 0; order < numWorks(); ++order)
		{
			const auto workId = workDispatchOrder[order];
			workers.queueWork(ManagedWork(WorkHandle(workId, this)));
		}
	}

	workers.waitAllWorks();

	/* Note: Do not wait on each flags here like this `m_workDoneFlags[workId].wait(false);` as 
	currently the flags are set before `notify_all`, we may not actually wait at all and deallocate
	the flag buffer, which may cause the call to `notify_all` in the work to crash. 
	*/

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
	PH_ASSERT_EQ(m_idToDependencyIds.size(), numWorks());

	PH_LOG(Workflow, Note, "building dispatch order from DAG for {} works", numWorks());

	// Builds a DAG by reversing the dependency lists
	auto workDAG          = std::make_unique<std::vector<std::size_t>[]>(numWorks());
	auto dependencyCounts = std::make_unique<int64[]>(numWorks());
	{
		std::size_t maxDependencies = 0;
		for(std::size_t workId = 0; workId < numWorks(); ++workId)
		{
			const std::vector<std::size_t>& dependencies = m_idToDependencyIds[workId];

			maxDependencies          = std::max(dependencies.size(), maxDependencies);
			dependencyCounts[workId] = dependencies.size();

			for(const std::size_t dependencyId : dependencies)
			{
				if(dependencyId == workId)
				{
					PH_LOG(Workflow, Warning,
						"work-{} depends on itself, ignoring the dependency", workId);
					continue;
				}

				if(dependencyId >= numWorks())
				{
					PH_LOG(Workflow, Warning,
						"work-{} referenced work-{} which is not tracked by the current Workflow, ignoring the dependency",
						workId, dependencyId);
					continue;
				}

				// Must have no self dependency
				PH_ASSERT_NE(workId, dependencyId);

				// Record that <workId> depends on <dependencyId> in DAG
				workDAG[dependencyId].push_back(workId);
			}
		}

		PH_LOG(Workflow, Note, "DAG building done, max dependencies/degree = {}", maxDependencies);
	}// end DAG building

	// Start topological sorting by finding works without any dependency
	std::queue<std::size_t> independentWorkIds;
	for(std::size_t workId = 0; workId < numWorks(); ++workId)
	{
		if(dependencyCounts[workId] == 0)
		{
			independentWorkIds.push(workId);
		}
	}

	PH_LOG(Workflow, Note, "{} works are already independent", independentWorkIds.size());

	// Main topological sorting that produces a valid work dispatch order
	auto        workDispatchOrder  = std::make_unique<std::size_t[]>(numWorks());
	std::size_t numDispatchedWorks = 0;
	while(!independentWorkIds.empty())
	{
		const auto independentWorkId = independentWorkIds.front();
		independentWorkIds.pop();

		workDispatchOrder[numDispatchedWorks++] = independentWorkId;

		// Mark the work as dispatched
		PH_ASSERT_EQ(dependencyCounts[independentWorkId], 0);
		dependencyCounts[independentWorkId] = -1;

		// Update the dependency count for works depending on the dispatched work
		// (remove dependency "edge")
		const std::vector<std::size_t>& dependingWorkIds = workDAG[independentWorkId];
		for(const std::size_t dependingWorkId : dependingWorkIds)
		{
			PH_ASSERT_GT(dependencyCounts[dependingWorkId], 0);
			dependencyCounts[dependingWorkId]--;

			// If the depending work is now independent, queue it for next dispatchment
			if(dependencyCounts[dependingWorkId] == 0)
			{
				independentWorkIds.push(dependingWorkId);
			}
		}
	}

	// Now a dispatch order that satisfies the dependencies between all works is established.
	// We check each work's dependency count to see if there is any error.

	for(std::size_t workId = 0; workId < numWorks(); ++workId)
	{
		const auto dependencyCount = dependencyCounts[workId];
		PH_ASSERT_NE(dependencyCount, 0);

		if(dependencyCount != -1)
		{
			PH_LOG(Workflow, Warning,
				"possible cyclic dependency detected: work-{} has {} dependencies that cannot be resolved",
				workId, dependencyCount);
		}
	}

	if(numDispatchedWorks != numWorks())
	{
		throw std::runtime_error(std::format(
			"dispatched {} works while there are {} in total",
			numDispatchedWorks, numWorks()));
	}

	return workDispatchOrder;
}

}// end namespace ph
