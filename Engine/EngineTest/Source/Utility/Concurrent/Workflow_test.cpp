#include <Utility/Concurrent/Workflow.h>
#include <Utility/Concurrent/FixedSizeThreadPool.h>

#include <gtest/gtest.h>

#include <atomic>
#include <cstddef>
#include <stdexcept>
#include <vector>

using namespace ph;

TEST(WorkflowTest, BasicUsagesWithoutRunning)
{
	// Creating works
	{
		Workflow workflow;

		const auto workA = workflow.addWork([](){});
		const auto workB = workflow.addWork([](){});
		const auto workC = workflow.addWork([](){});

		EXPECT_EQ(workflow.numWorks(), 3);
		EXPECT_EQ(workA.getWorkflow(), &workflow);
		EXPECT_EQ(workB.getWorkflow(), &workflow);
		EXPECT_EQ(workC.getWorkflow(), &workflow);
		EXPECT_NE(workA.getWorkId(), workB.getWorkId());
		EXPECT_NE(workB.getWorkId(), workC.getWorkId());
		EXPECT_NE(workA.getWorkId(), workC.getWorkId());
		
		const auto [workD, workE] = workflow.addWorks([](){}, [](){});
		EXPECT_EQ(workflow.numWorks(), 5);
		EXPECT_EQ(workD.getWorkflow(), &workflow);
		EXPECT_EQ(workE.getWorkflow(), &workflow);
	}

	// Work dependencies
	{
		Workflow workflow;

		// One by one
		auto workA = workflow.addWork([](){});
		auto workB = workflow.addWork([](){});
		workA.runsAfter(workB);

		// Multiple at once
		auto [workC, workD, workE] = workflow.addWorks([](){}, [](){}, [](){});
		workE.runsBefore<2>({workC, workD});

		// Multiple at once: simplified syntax
		workB.runsAfter(workflow.addWorks([](){}, [](){}, [](){}, [](){}));
	}
}

TEST(WorkflowTest, Running)
{
	// Simple summation without dependency
	{
		constexpr std::size_t NUM_INCREMENTS = 2000;

		std::atomic_uint32_t counter(0);

		Workflow wf(NUM_INCREMENTS);
		for(std::size_t i = 0; i < NUM_INCREMENTS; ++i)
		{
			wf.addWork([&counter]()
			{
				counter.fetch_add(1, std::memory_order_relaxed);
			});
		}

		FixedSizeThreadPool tp(12);
		wf.runAndWaitAllWorks(tp);

		EXPECT_EQ(counter.load(std::memory_order_relaxed), NUM_INCREMENTS);
	}

	// Simple dependency test with various graph sizes
	{
		auto summationTest = [](
			const std::size_t numGroups, 
			const std::size_t worksPerGroup,
			const std::size_t numWorkers)
		{
			const std::size_t numTotalWorks = numGroups * worksPerGroup;

			// Using plain `int` and not atomics for flags, as `Workflow` should ensure proper happens-before relation
			std::vector<int> doneFlags(numTotalWorks, 0);

			Workflow wf(numTotalWorks);

			// Split the increments into independent groups (execute sequentially within each group)
			std::vector<Workflow::WorkHandle> lastWorksInGroup(numGroups);
			for(std::size_t gi = 0; gi < numGroups; ++gi)
			{
				Workflow::WorkHandle previousWork;
				for(std::size_t i = 0; i < worksPerGroup; ++i)
				{
					const auto workIdx = gi * worksPerGroup + i;

					auto currentWork = wf.addWork([workIdx, i, worksPerGroup, &doneFlags]()
					{
						// Only mark current work as done if previous one has done
						if(i >= 1)
						{
							// Memory effect (setting the flag) done by previous work should be visible
							if(doneFlags[workIdx - 1] == 1)
							{
								doneFlags[workIdx] = 1;
							}
						}
						else
						{
							doneFlags[workIdx] = 1;
						}
					});

					// Setup dependencies

					if(i == worksPerGroup - 1)
					{
						lastWorksInGroup[gi] = currentWork;
					}

					if(i >= 1)
					{
						currentWork.runsAfter(previousWork);
					}

					previousWork = currentWork;
				}
			}

			// Finally sum all flags that has set (running the check after all groups are finished)
			std::size_t finalSum = 0;
			auto summationWork = wf.addWork([&doneFlags, &finalSum]()
			{
				for(std::size_t i = 0; i < doneFlags.size(); ++i)
				{
					finalSum += doneFlags[i];
				}
			});
			summationWork.runsAfter(lastWorksInGroup);
			
			FixedSizeThreadPool tp(numWorkers);
			wf.runAndWaitAllWorks(tp);

			EXPECT_EQ(finalSum, numTotalWorks);
		};// end test lambda
		
		// Baseline: smaller
		summationTest(1, 10, 1);
		summationTest(1, 10, 2);
		summationTest(2, 10, 1);
		summationTest(2, 10, 2);

		// Baseline: larger
		summationTest(10, 50, 12);

		// Test with different settings (smaller)
		for(std::size_t numGroups = 1; numGroups <= 5; numGroups += 1)
		{
			for(std::size_t worksPerGroup = 1; worksPerGroup <= 50; worksPerGroup += 10)
			{
				for(std::size_t numWorkers = 1; numWorkers <= 5; numWorkers += 1)
				{
					summationTest(numGroups, worksPerGroup, numWorkers);
				}
			}
		}

		// Test with different settings (larger)
		for(std::size_t numGroups = 10; numGroups <= 50; numGroups += 20)
		{
			for(std::size_t worksPerGroup = 100; worksPerGroup <= 1000; worksPerGroup += 200)
			{
				for(std::size_t numWorkers = 10; numWorkers <= 50; numWorkers += 20)
				{
					summationTest(numGroups, worksPerGroup, numWorkers);
				}
			}
		}
	}

	// Detect cyclic dependency
	{
		Workflow wf;
		auto workA = wf.addWork([](){});
		auto workB = wf.addWork([](){});
		workA.runsBefore(workB);
		workB.runsBefore(workA);

		FixedSizeThreadPool tp(4);
		EXPECT_THROW(wf.runAndWaitAllWorks(tp), std::runtime_error);
	}
}
