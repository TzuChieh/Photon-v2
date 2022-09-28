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
		constexpr std::size_t NUM_INCREMENTS = 4000;

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

	// Simple summation with dependency
	{
		auto summationTest = [](
			const std::size_t numGroups, 
			const std::size_t incrementsPerGroup,
			const std::size_t numWorkers)
		{
			const std::size_t numIncrements = numGroups * incrementsPerGroup;

			//std::vector<int> doneFlags(numIncrements, 0);
			std::vector<std::unique_ptr<std::atomic<int>>> doneFlags(numIncrements);
			for(auto& atomicPtr : doneFlags)
			{
				atomicPtr = std::make_unique<std::atomic<int>>(0);
			}

			Workflow wf(numIncrements);

			// Split the increments into independent groups (increment sequentially within each group)
			std::vector<Workflow::WorkHandle> lastWorksInGroup(numGroups);
			for(std::size_t gi = 0; gi < numGroups; ++gi)
			{
				Workflow::WorkHandle previousWork;
				for(std::size_t i = 0; i < incrementsPerGroup; ++i)
				{
					const auto workIdx = gi * incrementsPerGroup + i;

					auto currentWork = wf.addWork([workIdx, i, incrementsPerGroup, &doneFlags]()
					{
						// Only mark current work as done if previous one has done
						if(i >= 1)
						{
							//if(doneFlags[workIdx - 1] == 1)
							{
								*doneFlags[workIdx] = 1;
							}
						}
						else
						{
							*doneFlags[workIdx] = 1;
						}
					});

					// Setup dependencies

					if(i == incrementsPerGroup - 1)
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

			//std::mutex mmm;

			// Finally sum all flags that has set (running the check after all groups are finished)
			std::size_t finalSum = 0;
			auto summationWork = wf.addWork([&doneFlags, &finalSum]()
			{
				//std::lock_guard<std::mutex> lock(mmm);

				for(std::size_t i = 0; i < doneFlags.size(); ++i)
				{
					//EXPECT_EQ(doneFlags[i], 1) << i;

					finalSum += *doneFlags[i];
				}
			});
			summationWork.runsBefore(lastWorksInGroup);
			
			{
				FixedSizeThreadPool tp(numWorkers);
				wf.runAndWaitAllWorks(tp);

				//std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}

			//std::lock_guard<std::mutex> lock(mmm);
			EXPECT_EQ(finalSum, numIncrements);
		};// end test lambda
		
		// Baseline: smaller
		//summationTest(1, 100, 1);
		//summationTest(1, 100, 2);
		//summationTest(2, 100, 1);
		//summationTest(2, 100, 2);

		//// Baseline: larger
		//summationTest(10, 400, 12);

		// Test with different settings (smaller)
		for(std::size_t numGroups = 1; numGroups <= 10; numGroups += 1)
		{
			for(std::size_t incrementsPerGroup = 1; incrementsPerGroup <= 100; incrementsPerGroup += 10)
			{
				for(std::size_t numWorkers = 1; numWorkers <= 10; ++numWorkers)
				{
					summationTest(numGroups, incrementsPerGroup, numWorkers);
				}
			}
		}

		// TODO: the bug seems to be we are using more works than groups... essentially there will be
		// some counters being incremented concurrently... right?
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
