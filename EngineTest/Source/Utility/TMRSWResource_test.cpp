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
	ph::TMRSWResource<TestStruct> copiedResource1(testStruct);
	ph::TMRSWResource<TestStruct> movedResource1(std::move(testStruct));

	auto testUniquePtr = std::make_unique<TestStruct>();
	ph::TMRSWResource<std::unique_ptr<TestStruct>> movedResource2(std::move(testUniquePtr));
}

TEST(TMRSWResourceTest, ExclusivelyUseResource)
{
	std::size_t sum = 0;
	ph::TMRSWResource<std::size_t*> sumResource(&sum);
	
	constexpr std::size_t NUM_THREADS               = 4;
	constexpr std::size_t NUM_INCREMENTS_PER_THREAD = 100;

	std::thread threads[NUM_THREADS];
	for(std::thread& thread : threads)
	{
		thread = std::thread([=, &sumResource]
		{
			for(std::size_t i = 0; i < NUM_INCREMENTS_PER_THREAD; ++i)
			{
				sumResource.write([](std::size_t* sum)
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
	/*int number1 = 3;	
	ph::TMRSWResource<int*> resource1(&number1);
	resource1.directCall([](int* number)
	{
		*number = 7;
	});
	EXPECT_EQ(number1, 7);*/

	auto number2 = std::make_shared<float>(-9.0f);
	//auto b = number2;
	
	ph::TMRSWResource<std::shared_ptr<float>> resource2(number2);
	resource2.directCall([](std::shared_ptr<float>& number)
	{
		(*number) += 10.0f;
	});
	//std::cerr << *b << std::endl;
	//std::cerr << *number2 << std::endl;
	//bool a = *number2 == 1.0f;

	EXPECT_EQ(*number2, 1.0f);
}