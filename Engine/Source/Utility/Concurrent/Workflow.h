#pragma once

#include "Utility/INoCopyAndMove.h"
#include "Utility/TSpan.h"

#include <vector>
#include <cstddef>
#include <functional>
#include <array>
#include <utility>
#include <memory>
#include <atomic>
#include <tuple>
#include <utility>

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
		WorkHandle();

		void runsBefore(WorkHandle succeedingWork);
		void runsAfter(WorkHandle preceedingWork);
		void runsBefore(TSpanView<WorkHandle> succeedingWorks);
		void runsAfter(TSpanView<WorkHandle> preceedingWorks);

		template<std::size_t N>
		void runsBefore(std::array<WorkHandle, N> succeedingWorks);

		template<std::size_t N>
		void runsAfter(std::array<WorkHandle, N> preceedingWorks);

		std::size_t getWorkId() const;
		Workflow* getWorkflow() const;

		operator bool () const;

	private:
		WorkHandle(std::size_t workId, Workflow* workflow);

		std::size_t m_workId;
		Workflow*   m_workflow;
	};

public:
	Workflow();
	explicit Workflow(std::size_t numExpectedWorks);

	WorkHandle addWork(Work work);
	WorkHandle acquireWork(std::size_t workIndex);

	template<typename... WorkTypes>
	auto addWorks(WorkTypes&&... works);

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
	void startWorkflow();
	void finishWorkflow();
	std::unique_ptr<std::size_t[]> determineDispatchOrderFromTopologicalSort() const;

	std::vector<Work> m_works;

	/* An adjacency list recording the depending works of each work. The dependencies are recorded by
	corresponding work ID. E.g., if work "k" is the "j"-th dependency of work "i", the relation will be
	represented as "m_idToDependencyIds[i][j] = k".
	*/
	std::vector<std::vector<std::size_t>> m_idToDependencyIds;

	std::unique_ptr<std::atomic_flag[]> m_workDoneFlags;
};

// In-header Implementations:

template<typename... WorkTypes>
inline auto Workflow::addWorks(WorkTypes&&... works)
{
	return std::array{addWork(std::forward<WorkTypes>(works))...};
}

inline void Workflow::WorkHandle::runsBefore(TSpanView<WorkHandle> succeedingWorks)
{
	for(const WorkHandle work : succeedingWorks)
	{
		runsBefore(work);
	}
}

inline void Workflow::WorkHandle::runsAfter(TSpanView<WorkHandle> preceedingWorks)
{
	for(const WorkHandle work : preceedingWorks)
	{
		runsAfter(work);
	}
}

template<std::size_t N>
inline void Workflow::WorkHandle::runsBefore(std::array<WorkHandle, N> succeedingWorks)
{
	runsBefore(TSpanView<WorkHandle>(succeedingWorks));
}

template<std::size_t N>
inline void Workflow::WorkHandle::runsAfter(std::array<WorkHandle, N> preceedingWorks)
{
	runsAfter(TSpanView<WorkHandle>(preceedingWorks));
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
