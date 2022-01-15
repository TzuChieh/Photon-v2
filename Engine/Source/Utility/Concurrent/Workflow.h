#pragma once

#include "Utility/INoCopyAndMove.h"

#include <vector>
#include <cstddef>
#include <functional>
#include <array>
#include <utility>
#include <memory>
#include <atomic>

namespace ph { class FixedSizeThreadPool; }

namespace ph
{

class Workflow final : private INoCopyAndMove
{
public:
	using Work = std::function<void()>;

	/*! @brief Lightweight handle for an added work.
	*/
	class WorkHandle final
	{
		friend class Workflow;

	public:
		void runsBefore(WorkHandle succeedingWork);
		void runsAfter(WorkHandle preceedingWork);

		template<std::size_t N>
		void runsBefore(std::array<WorkHandle, N> succeedingWorks);

		template<std::size_t N>
		void runsAfter(std::array<WorkHandle, N> preceedingWorks);

		std::size_t getWorkId() const;
		Workflow* getWorkflow() const;

		operator bool () const;

	private:
		WorkHandle();
		WorkHandle(std::size_t workId, Workflow* workflow);

		std::size_t m_workId;
		Workflow*   m_workflow;
	};

public:
	Workflow();
	explicit Workflow(std::size_t numExpectedWorks);

	WorkHandle addWork(Work work);

	template<std::size_t N>
	std::array<WorkHandle, N> addWorks(std::array<Work, N> works);

	void dependsOn(WorkHandle target, WorkHandle targetDependency);
	void runAndWaitAllWorks(FixedSizeThreadPool& workers);

	std::size_t numWorks() const;

private:
	class ManagedWork final
	{
	public:
		explicit ManagedWork(WorkHandle handle);

		void operator () ();

	private:
		WorkHandle m_handle;
	};

	void ensureValidWorkHandle(WorkHandle work) const;

	std::vector<Work>                     m_works;
	std::vector<std::vector<std::size_t>> m_idToDependencyIds;
	std::unique_ptr<std::atomic_flag[]>   m_workDoneFlags;
};

// In-header Implementations:

template<std::size_t N>
inline std::array<Workflow::WorkHandle, N> Workflow::addWorks(std::array<Work, N> works)
{
	std::array<WorkHandle, N> workHandles;
	for(std::size_t i = 0; i < N; ++i)
	{
		workHandles[i] = addWork(std::move(works[i]));
	}
	return workHandles;
}

template<std::size_t N>
inline void Workflow::WorkHandle::runsBefore(std::array<WorkHandle, N> succeedingWorks)
{
	for(WorkHandle work : succeedingWorks)
	{
		runsBefore(std::move(work));
	}
}

template<std::size_t N>
inline void Workflow::WorkHandle::runsAfter(std::array<WorkHandle, N> preceedingWorks)
{
	for(WorkHandle work : preceedingWorks)
	{
		runsAfter(std::move(work));
	}
}

inline std::size_t Workflow::WorkHandle::getWorkId() const
{
	return m_workId;
}

inline Workflow* Workflow::WorkHandle::getWorkflow() const
{
	return m_workflow;
}

inline Workflow::WorkHandle::operator bool () const
{
	return m_workId != static_cast<std::size_t>(-1) && m_workflow != nullptr;
}

}// end namespace ph
