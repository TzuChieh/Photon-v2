#include <Utility/Concurrent/Workflow.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(WorkflowTest, BasicUsagesWithoutRunning)
{
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
		
		const auto works = workflow.addWorks([](){}, [](){});
	}
}
