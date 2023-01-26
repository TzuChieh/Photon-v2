#include <Utility/Concurrent/TSynchronized.h>

#include <gtest/gtest.h>

#include <string>
#include <thread>
#include <cstdint>

using namespace ph;

TEST(TSynchronizedTest, SingleThreadUsage)
{
	{
		TSynchronized<int> sInt;
		sInt = 3;
		EXPECT_EQ(sInt.makeCopy(), 3);
	}

	{
		TSynchronized<std::string> sStr("12345");
		EXPECT_STREQ(sStr->c_str(), "12345");
	}

	{
		TSynchronized<float> sFlt(0.0f);
		for(int i = 1; i <= 20; ++i)
		{
			sFlt = static_cast<float>(i);
		}
		EXPECT_EQ(sFlt.makeCopy(), 20);
	}
}

TEST(TSynchronizedTest, MultiThreadUsage)
{
	// Threads incrementing integer
	{
		using Int = std::uint64_t;

		constexpr Int numThreads = 20;
		constexpr Int numIncrementsPerThread = 100000;

		TSynchronized<Int> sInt(0);

		// All threads start incrementing `sInt` for `numIncrementsPerThread` times
		std::thread threads[numThreads];
		for(Int ti = 0; ti < numThreads; ++ti)
		{
			threads[ti] = std::thread(
				[&sInt, numIncrementsPerThread]()
				{
					for(Int i = 0; i < numIncrementsPerThread; ++i)
					{
						*(sInt.lockedPtr()) += 1;
					}
				});
		}

		// Wait for all threads
		for(Int ti = 0; ti < numThreads; ++ti)
		{
			threads[ti].join();
		}

		EXPECT_EQ(sInt.makeCopy(), numThreads * numIncrementsPerThread);
	}
}
