#include <Utility/TMRSWResource.h>

#include <gtest/gtest.h>

#include <memory>
#include <utility>
#include <thread>
#include <cstddef>

TEST(TMRSWResourceTest, ResourceCopyAndMoveInit)
{
	struct TestStruct
	{
		int a;
		float b;
		double c;
	};

	TestStruct testStruct;
	ph::TMRSWResource copiedResource1(testStruct);
	ph::TMRSWResource movedResource1(std::move(testStruct));

	auto testUniquePtr = std::make_unique<TestStruct>();
	ph::TMRSWResource movedResource2(std::move(testUniquePtr));
}

TEST(TMRSWResourceTest, ExclusivelyUseResource)
{
	std::size_t sum = 0;
	ph::TMRSWResource numResource(&sum);
	
	constexpr std::size_t NUM_THREADS               = 4;
	constexpr std::size_t NUM_INCREMENTS_PER_THREAD = 100;

	std::thread threads[NUM_THREADS];
	for(std::thread& thread : threads)
	{
		thread = std::thread([=, &numResource]
		{
			for(std::size_t i = 0; i < NUM_INCREMENTS_PER_THREAD; ++i)
			{
				numResource.write([](std::size_t* const sum)
				{
					++(*sum);
				});
			}
		});
	}

	for(std::thread& thread : threads)
	{
		thread.join();
	}

	// a total of <NUM_THREADS>*<NUM_INCREMENTS_PER_THREAD> additions are performed
	EXPECT_EQ(sum, NUM_THREADS * NUM_INCREMENTS_PER_THREAD);
}

TEST(TMRSWResourceTest, DirectlyUseResource)
{
	int number1 = 3;	
	ph::TMRSWResource resource1(&number1);
	resource1.directCall([](int* const number)
	{
		*number = 7;
	});
	EXPECT_EQ(number1, 7);

	auto number2 = std::make_shared<float>(-9.0f);
	ph::TMRSWResource resource2(number2);
	resource2.directCall([](std::shared_ptr<float>& number)
	{
		(*number) += 10.0f;
	});
	EXPECT_EQ(*number2, 1.0f);
}