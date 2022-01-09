#pragma once

#include <vector>
#include <cstddef>
#include <functional>
#include <initializer_list>

namespace ph
{

class Workflow final
{
public:
	using Work = std::function<void()>;

	class WorkHandle final
	{
		friend class Workflow;

	public:
		void runsBefore(WorkHandle succeedingWork);
		void runsAfter(WorkHandle preceedingWork);
		void runsBefore(std::initializer_list<WorkHandle> succeedingWorks);
		void runsAfter(std::initializer_list<WorkHandle> preceedingWorks);

	private:
		WorkHandle(std::size_t workId, Workflow* workflow);

		std::size_t m_workId;
		Workflow*   m_workflow;
	};

public:
	Workflow();
	explicit Workflow(std::size_t numExpectedWorks);

	WorkHandle addWork(Work work);
	WorkHandle addWorks(std::initializer_list<Work> works);
	std::size_t numWorks() const;

private:
	std::vector<Work>                     m_works;
	std::vector<std::vector<std::size_t>> m_idToDependentIds;
};

}// end namespace ph
