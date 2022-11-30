#include <EditorCore/Thread/TUnbufferedFrameWorkerThread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <algorithm>

using namespace ph::editor;

namespace
{

template<typename WorkSignature>
class TMockUnbufferedFrameWorker : public TUnbufferedFrameWorkerThread<WorkSignature>
{
	using Base = TUnbufferedFrameWorkerThread<WorkSignature>;
	using Work = Base::Work;

public:
	MOCK_METHOD(void, onAsyncProcessWork, (const Work& work), (override));
	MOCK_METHOD(void, onBeginFrame, (), (override));
	MOCK_METHOD(void, onEndFrame, (), (override));
};

}

TEST(TUnbufferedFrameWorkerThreadTest, RunSingleFrame)
{
	// Smallest possible work duration is running a single frame then exit

	// No work
	{
		// Test for 100 times to reveal possible threading error
		for(int i = 0; i < 100; ++i)
		{
			TMockUnbufferedFrameWorker<void(void)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(0);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

			worker.startWorker();
			worker.beginFrame();

			// adds no work...

			worker.requestWorkerStop();
			worker.endFrame();
			worker.waitForWorkerToStop();
		}
	}

	// Incrementally add small works
	{
		// Test for 100 times to reveal possible threading error
		for(int i = 0; i < 100; ++i)
		{
			const int numWorksToAdd = i;

			TMockUnbufferedFrameWorker<int(int)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(numWorksToAdd);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

			worker.startWorker();
			worker.beginFrame();

			int someOtherNumber = i * 7;
			int someResult = 0;

			// adds exactly `i` works
			for(int j = 0; j < numWorksToAdd; ++j)
			{
				worker.addWork(
					[j, &someOtherNumber, &someResult](int num)
					{
						// do random useless things
						someOtherNumber = num + 3 / 5 * someResult;
						someResult += num * j;
						return someResult + someOtherNumber;
					});
			}

			worker.requestWorkerStop();
			worker.endFrame();
			worker.waitForWorkerToStop();
		}
	}

	// Incrementally add large works
	{
		constexpr std::size_t NUM_TIMES = 100;

		// Test for `NUM_TIMES` times to reveal possible threading error
		for(int i = 0; i < NUM_TIMES; ++i)
		{
			const int numWorksToAdd = i;

			TMockUnbufferedFrameWorker<int(int)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(numWorksToAdd);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

			worker.startWorker();
			worker.beginFrame();

			std::array<float, 500> someData;
			someData.fill(i * 123.0f);
			float someResult = 0.0f;

			// adds exactly `i` works
			for(int j = 0; j < numWorksToAdd; ++j)
			{
				worker.addWork(
					[j, someData, &someResult](int num)
					{
						// do random useless things
						someResult += someData[j] + 3 / 5 + num * j;
						return static_cast<int>(someResult + someData[j]);
					});
			}

			worker.requestWorkerStop();
			worker.endFrame();
			worker.waitForWorkerToStop();
		}
	}
}

TEST(TUnbufferedFrameWorkerThreadTest, RunMultipleFrames)
{
	constexpr std::size_t MAX_FRAMES = 20;

	// Test from 1 frame to `MAX_FRAMES` frames to reveal possible threading error
	for(int numFrames = 1; numFrames <= MAX_FRAMES; ++numFrames)
	{
		const int numSmallWorksToAdd = numFrames * 5;
		const int numLargeWorksToAdd = numFrames * 2;

		TMockUnbufferedFrameWorker<void(int, int, int)> worker;

		EXPECT_CALL(worker, onAsyncProcessWork)
			.Times(numFrames * (numSmallWorksToAdd + numLargeWorksToAdd));
		EXPECT_CALL(worker, onBeginFrame)
			.Times(numFrames);
		EXPECT_CALL(worker, onEndFrame)
			.Times(numFrames);

		worker.startWorker();

		// Run `numFrames` frames
		for(int fi = 0; fi < numFrames; ++fi)
		{
			worker.beginFrame();

			std::array<double, 128> largeData;
			largeData.fill(fi * 55.0);
			int someResult = 0;

			// Add small works
			for(int wi = 0; wi < numSmallWorksToAdd; ++wi)
			{
				worker.addWork(
					[wi, &someResult](int num1, int num2, int num3)
					{
						// do random useless things
						someResult += num1 + num2 * num3;
						someResult *= wi;
					});
			}

			// Add large works
			for(int wi = 0; wi < numLargeWorksToAdd; ++wi)
			{
				worker.addWork(
					[wi, largeData, &someResult](int num1, int num2, int num3)
					{
						// do random useless things
						someResult += num1 + num2 * num3 + static_cast<int>(largeData[wi % largeData.size()]);
						someResult *= wi;
					});
			}

			// Request stop on last frame
			if(fi == numFrames - 1)
			{
				worker.requestWorkerStop();
			}
				
			worker.endFrame();
		}

		worker.waitForWorkerToStop();
	}
}

TEST(TUnbufferedFrameWorkerThreadTest, RunSmartPtrCaptureWork)
{
	// Incrementally add small works, unbuffered
	{
		struct Counter
		{
			int& count;

			Counter(int& count)
				: count(count)
			{}

			~Counter()
			{
				++count;
			}
		};

		// Test for 100 times to reveal possible threading error
		for(int i = 0; i < 100; ++i)
		{
			const int numWorksToAdd = i;

			TMockUnbufferedFrameWorker<void(void)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(numWorksToAdd);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

			worker.startWorker();
			worker.beginFrame();

			int count = 0;

			// adds exactly `i` works
			for(int j = 0; j < numWorksToAdd; ++j)
			{
				// add `shared_ptr` & `unique_ptr` works based on even/odd `j`

				if(j % 2 == 0)
				{
					worker.addWork(
						[ptr = std::make_unique<Counter>(count)]()
						{
							ASSERT_TRUE(ptr);
						});
				}
				else
				{
					worker.addWork(
						[ptr = std::make_shared<Counter>(count)]()
						{
							ASSERT_TRUE(ptr);
						});
				}
			}

			worker.requestWorkerStop();
			worker.endFrame();
			worker.waitForWorkerToStop();

			// `Counter` should be destructed `numWorksToAdd` times
			EXPECT_EQ(count, numWorksToAdd);
		}
	}
}

TEST(TUnbufferedFrameWorkerThreadTest, WorkObjectDestruct)
{
	// Mixing shared ptr and unique ptr
	{
		for(int i = 1; i < 10; ++i)
		{
			const int numWorksPerFrame = i;
			const int numFramesToRun = i;

			// Note: this is a buffered worker
			TMockUnbufferedFrameWorker<void(void)> worker;
			worker.startWorker();

			int deleteCount = 0;
			auto ptrDeleter =
				[&worker, &deleteCount](int* const ptr)
				{
					// Must be called on worker thread
					EXPECT_EQ(worker.getWorkerThreadId(), std::this_thread::get_id());

					delete ptr;

					++deleteCount;
				};

			for(int fi = 0; fi < numFramesToRun; ++fi)
			{
				worker.beginFrame();

				// adds exactly `i` works
				for(int wi = 0; wi < numWorksPerFrame; ++wi)
				{
					// add `shared_ptr` & `unique_ptr` works based on even/odd `wi`

					if(wi % 2 == 0)
					{
						worker.addWork(
							[ptr = std::unique_ptr<int, decltype(ptrDeleter)>(new int(), ptrDeleter)]()
							{
								ASSERT_TRUE(ptr);
							});
					}
					else
					{
						worker.addWork(
							[ptr = std::shared_ptr<int>(new int(), ptrDeleter)]()
							{
								ASSERT_TRUE(ptr);
							});
					}
				}

				// Request stop on last frame
				if(fi == numFramesToRun - 1)
				{
					worker.requestWorkerStop();
				}

				worker.endFrame();
			}
			
			worker.waitForWorkerToStop();

			// All works should still be destructed to ensure correctness
			const auto numTotalWorks = numWorksPerFrame * numFramesToRun;
			EXPECT_EQ(deleteCount, numTotalWorks);
		}
	}
}
