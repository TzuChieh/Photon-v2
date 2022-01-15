#pragma once

#include "Utility/Concurrent/Workflow.h"
#include "Common/assertion.h"
#include "Utility/Concurrent/FixedSizeThreadPool.h"

#include <stdexcept>
#include <algorithm>

namespace ph
{

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

//Workflow::ControlledWork::ControlledWork(Work work, Workflow* const workflow) :
//	m_work(std::move(work)), m_workflow(workflow)
//{
//	PH_ASSERT(m_work);
//	PH_ASSERT(m_workflow);
//}
//
//void Workflow::ControlledWork::operator () ()
//{
//	m_work();
//}

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
	m_workDoneFlags = std::make_unique<std::atomic_flag[]>(numWorks());

	// TODO: topo sort
	for(std::size_t workId = 0; workId < numWorks(); ++workId)
	{
		workers.queueWork(ManagedWork(WorkHandle(workId, this)));
	}

	workers.waitAllWorks();

	// We do not need the flags anymore, release memory
	m_workDoneFlags = nullptr;
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

}// end namespace ph
