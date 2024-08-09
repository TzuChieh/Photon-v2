#include <Utility/Concurrent/InitiallyPausedThread.h>

#include <gtest/gtest.h>

#include <mutex>
#include <chrono>
#include <thread>

#if !GTEST_IS_THREADSAFE 
	#error "`InitiallyPausedThreadTest` requires googletest to be thread safe."
#endif

using namespace ph;

TEST(InitiallyPausedThreadTest, ConstructWithoutRunning)
{
	// Default-constructed, does not represent a thread
	{
		InitiallyPausedThread thread;

		EXPECT_FALSE(thread.hasStarted());
		EXPECT_FALSE(thread.hasJoined());
	}

	// Some random task
	{
		InitiallyPausedThread thread([](){});

		EXPECT_FALSE(thread.hasStarted());
		EXPECT_FALSE(thread.hasJoined());
	}

	// Some random task
	{
		double value = 777.0;
		InitiallyPausedThread thread([](int x, double y){}, 3, value);

		EXPECT_FALSE(thread.hasStarted());
		EXPECT_FALSE(thread.hasJoined());
	}
}

TEST(InitiallyPausedThreadTest, Running)
{
	{
		std::mutex mutex;

		int value = 0;
		InitiallyPausedThread thread(
			[&mutex](int* valuePtr)
			{
				std::lock_guard<std::mutex> lock(mutex);

				(*valuePtr)++;
			},
			&value);

		// Sleep for 1 second so the chance of accidentally passing the test is smaller
		std::this_thread::sleep_for(std::chrono::seconds(1));

		// Thread should be in paused state--value not being set
		{
			std::lock_guard<std::mutex> lock(mutex);
			EXPECT_EQ(value, 0);
		}

		EXPECT_FALSE(thread.hasStarted());
		EXPECT_FALSE(thread.hasJoined());

		thread.start();
		thread.join();
		
		EXPECT_TRUE(thread.hasStarted());
		EXPECT_TRUE(thread.hasJoined());

		// Thread has done its work--value being incremented exactly 1 time
		{
			std::lock_guard<std::mutex> lock(mutex);
			EXPECT_EQ(value, 1);
		}
	}
}

TEST(InitiallyPausedThreadTest, StartStateVisibility)
{
	for(int i = 0; i < 100; ++i)
	{
		InitiallyPausedThread thread;

		auto func = 
			[&thread]()
			{
				// Should be immediately visible in the called functor
				EXPECT_TRUE(thread.hasStarted());
			};

		thread = InitiallyPausedThread(func);
		thread.start();
		
		EXPECT_TRUE(thread.hasStarted());

		thread.join();
	}
}
