#include <Utility/Concurrent/Workflow.h>
#include <Utility/Concurrent/FixedSizeThreadPool.h>

#include <gtest/gtest.h>

#include <atomic>
#include <cstddef>
#include <array>
#include <stdexcept>

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
		constexpr std::size_t NUM_INCREMENTS = 4000;

		std::atomic_uint64_t counter(0);

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

	// Simple summation with dependency
	{
		constexpr std::size_t NUM_INCREMENTS       = 4000;
		constexpr std::size_t NUM_GROUPS           = 10;
		constexpr std::size_t INCREMENTS_PER_GROUP = NUM_INCREMENTS / NUM_GROUPS;

		ASSERT_EQ(NUM_GROUPS * INCREMENTS_PER_GROUP, NUM_INCREMENTS);

		// Init to 0
		std::size_t counters[NUM_GROUPS] = {};

		Workflow wf(NUM_INCREMENTS);

		// Split the increments into independent groups (increment sequentially within each group)
		std::array<Workflow::WorkHandle, NUM_GROUPS> lastWorksInGroup;
		for(std::size_t gi = 0; gi < NUM_GROUPS; ++gi)
		{
			for(std::size_t i = 0; i < INCREMENTS_PER_GROUP; ++i)
			{
				auto currentWork = wf.addWork([counter = &counters[gi]]()
				{
					*counter += 1;
				});

				if(i == INCREMENTS_PER_GROUP - 1)
				{
					lastWorksInGroup[gi] = currentWork;
				}

				if(i >= 1)
				{
					const auto previousWork = wf.acquireWork(gi * INCREMENTS_PER_GROUP + i - 1);
					currentWork.runsAfter(previousWork);
				}
			}

			ASSERT_TRUE(lastWorksInGroup[gi]);
		}

		// Finally sum all groups (running the summation after all groups are finished)
		std::size_t finalSum = 0;
		auto summationWork = wf.addWork([&counters, &finalSum]()
		{
			for(std::size_t gi = 0; gi < NUM_GROUPS; ++gi)
			{
				finalSum += counters[gi];
			}
		});
		summationWork.runsAfter(lastWorksInGroup);

		FixedSizeThreadPool tp(12);
		wf.runAndWaitAllWorks(tp);

		EXPECT_EQ(finalSum, NUM_INCREMENTS);
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