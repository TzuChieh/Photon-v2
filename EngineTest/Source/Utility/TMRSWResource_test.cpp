#include <Utility/TMRSWResource.h>

#include <gtest/gtest.h>

#include <memory>
#include <utility>
#include <thread>
#include <cstddef>
#include <type_traits>

using namespace ph;

namespace
{

struct TestStruct
{};

}

TEST(TMRSWResourceTest, RequiredProperties)
{
	{
		using ResType = TMRSWResource<TestStruct>;
		EXPECT_FALSE(std::is_copy_constructible_v<ResType>);
		EXPECT_FALSE(std::is_copy_assignable_v<ResType>);
		EXPECT_FALSE(std::is_move_constructible_v<ResType>);
		EXPECT_FALSE(std::is_move_assignable_v<ResType>);
	}
}

TEST(TMRSWResourceTest, ResourceCopyAndMoveInit)
{
	TestStruct testObj;
	TMRSWResource<TestStruct> copiedResource1(testObj);
	TMRSWResource<TestStruct> movedResource1(std::move(testObj));

	auto testUniquePtr = std::make_unique<TestStruct>();
	TMRSWResource<std::unique_ptr<TestStruct>> movedResource2(std::move(testUniquePtr));
	EXPECT_FALSE(testUniquePtr);

	auto testSharedPtr = std::make_shared<TestStruct>();
	TMRSWResource<std::shared_ptr<TestStruct>> copiedResource2(testSharedPtr);
	EXPECT_TRUE(testSharedPtr);
}

TEST(TMRSWResourceTest, ExclusivelyUseResource)
{
	std::size_t sum = 0;
	TMRSWResource<std::size_t*> sumResource(&sum);
	
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
	int number1 = 3;	
	TMRSWResource<int*> resource1(&number1);
	resource1.directCall([](int* number)
	{
		*number = 7;
	});
	EXPECT_EQ(number1, 7);

	auto number2 = std::make_shared<float>(-9.0f);
	TMRSWResource<std::shared_ptr<float>> resource2(number2);
	resource2.directCall([](std::shared_ptr<float>& number)
	{
		(*number) += 10.0f;
	});
	ASSERT_TRUE(number2);
	EXPECT_EQ(*number2, 1.0f);
}
