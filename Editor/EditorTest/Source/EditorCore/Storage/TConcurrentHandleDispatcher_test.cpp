#include <EditorCore/Storage/TConcurrentHandleDispatcher.h>

#include <gtest/gtest.h>

#include <vector>
#include <thread>
#include <unordered_set>
#include <mutex>

#if !GTEST_IS_THREADSAFE 
	#error "TConcurrentHandleDispatcherTest requires googletest to be thread safe."
#endif

using namespace ph::editor;

TEST(TConcurrentHandleDispatcherTest, SingleThreadDispatchment)
{
	// Dispatch and return
	{
		using Handle = TWeakHandle<int>;

		constexpr int numHandles = 1000;

		TConcurrentHandleDispatcher<Handle> dispatcher;
		std::vector<Handle> handles(numHandles);
		for(int i = 0; i < numHandles; ++i)
		{
			auto handle = dispatcher.dispatchOne();
			EXPECT_TRUE(handle);

			// Never dispatch duplicated handles
			EXPECT_FALSE(handles[handle.getIndex()]);
			handles[handle.getIndex()] = handle;
		}

		// Return all dispatched handles
		for(int i = 0; i < numHandles; ++i)
		{
			// Manually renew a handle
			auto nextGen = Handle::nextGeneration(handles[i].getGeneration());
			dispatcher.returnOne(Handle(handles[i].getIndex(), nextGen));
		}
		
		// Dispatch again, this time should all be renewed handles (with a different generation)
		for(int i = 0; i < numHandles; ++i)
		{
			auto handle = dispatcher.dispatchOne();
			EXPECT_TRUE(handle);

			EXPECT_TRUE(handles[handle.getIndex()] != handle);
			EXPECT_NE(handles[handle.getIndex()].getGeneration(), handle.getGeneration());
			handles[handle.getIndex()] = handle;
		}
	}
}

TEST(TConcurrentHandleDispatcherTest, MultiThreadDispatchment)
{
	{
		using Handle = TWeakHandle<int>;

		constexpr int numThreads = 20;
		constexpr int numOpsPerThread = 10000;

		TConcurrentHandleDispatcher<Handle> dispatcher;
		std::unordered_set<Handle> handleSet;
		std::vector<Handle> handleCache;
		std::mutex mutex;

		// Dispatching and returning handles concurrently.
		// A set is storing the dispatched handles and no collision should happen there.
		{
			std::vector<std::jthread> threads(numThreads);
			for(int ti = 0; ti < numThreads; ++ti)
			{
				// Even threads are dispatching
				if(ti % 2 == 0)
				{
					threads[ti] = std::jthread(
						[&dispatcher, &handleSet, &handleCache, &mutex]()
						{
							for(int i = 0; i < numOpsPerThread; ++i)
							{
								auto handle = dispatcher.dispatchOne();
								EXPECT_TRUE(handle);

								{
									std::lock_guard lock(mutex);
									EXPECT_FALSE(handleSet.contains(handle));
									handleSet.insert(handle);
									handleCache.push_back(handle);
								}
							}
						});
				}
				// Odd threads are returning
				else
				{
					threads[ti] = std::jthread(
						[ti, &dispatcher, &handleCache, &mutex]()
						{
							for(int i = 0; i < numOpsPerThread; ++i)
							{
								Handle handle;
								{
									std::lock_guard lock(mutex);

									// Select a handle to renew base on thread and op index
									const auto handleIdx = ti * i + ti;
									if(handleIdx < handleCache.size() && handleCache[handleIdx])
									{
										Handle& cachedHandle = handleCache[handleIdx];

										// Manually renew a handle
										auto nextGen = Handle::nextGeneration(cachedHandle.getGeneration());
										handle = Handle(cachedHandle.getIndex(), nextGen);

										// Invalidate so we will not dispatch duplicated handles
										cachedHandle = Handle();
									}
								}

								if(handle)
								{
									dispatcher.returnOne(handle);
								}
							}
						});
				}
			}
		}// jthreads joined
	}
}
