#include <EditorCore/Thread/TFrameWorkerThread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <algorithm>

using namespace ph::editor;

using testing::Between;

namespace
{

template<std::size_t NUM_BUFFERS, typename WorkSignature>
class TMockFrameWorker : public TFrameWorkerThread<NUM_BUFFERS, WorkSignature>
{
	using Base = TFrameWorkerThread<NUM_BUFFERS, WorkSignature>;
	using Work = Base::Work;

public:
	MOCK_METHOD(void, onAsyncProcessWork, (const Work& work), (override));
	MOCK_METHOD(void, onBeginFrame, (std::size_t frameNumber), (override));
	MOCK_METHOD(void, onEndFrame, (), (override));
};

}

TEST(TFrameWorkerThreadTest, RunSingleFrameUnbuffered)
{
	// Smallest possible work duration is running a single frame then exit

	// No work and no buffer
	{
		// Test for 1000 times to reveal possible threading error
		for(int i = 0; i < 1000; ++i)
		{
			TMockFrameWorker<1, void(void)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(0);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

			worker.beginFrame();

			// adds no work...

			worker.requestWorkerStop();
			worker.endFrame();
			worker.waitForWorkerToStop();
		}
	}

	// Incrementally add small works
	{
		// Test for 1000 times to reveal possible threading error
		for(int i = 0; i < 1000; ++i)
		{
			const int numWorksToAdd = i;

			TMockFrameWorker<1, int(int)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(numWorksToAdd);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

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
		constexpr std::size_t NUM_TIMES = 1000;

		// Test for `NUM_TIMES` times to reveal possible threading error
		for(int i = 0; i < NUM_TIMES; ++i)
		{
			const int numWorksToAdd = i;

			TMockFrameWorker<1, int(int)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(numWorksToAdd);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

			worker.beginFrame();

			std::array<float, NUM_TIMES> someData;
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

namespace
{

template<std::size_t NUM_BUFFERS>
inline void run_multiple_frames_buffered_test()
{
	constexpr std::size_t MAX_FRAMES = 50;

	// Test from 1 frame to `MAX_FRAMES` frames to reveal possible threading error
	for(int numFrames = 1; numFrames <= MAX_FRAMES; ++numFrames)
	{
		const int numSmallWorksToAdd = numFrames * 5;
		const int numLargeWorksToAdd = numFrames * 2;

		TMockFrameWorker<NUM_BUFFERS, void(int, int, int)> worker;

		// For N buffered frames, we may process every work (#frames * #works); or at most, skipped
		// N frames (all buffering frames including the current frame do not get to be processed, works 
		// processed = (#frames - N) * #works)
		const int maxSkippedFrames = std::min(numFrames, static_cast<int>(NUM_BUFFERS));

		EXPECT_CALL(worker, onAsyncProcessWork)
			.Times(Between(
				(numFrames - maxSkippedFrames) * (numSmallWorksToAdd + numLargeWorksToAdd),
				(numFrames) * (numSmallWorksToAdd + numLargeWorksToAdd)));

		EXPECT_CALL(worker, onBeginFrame)
			.Times(numFrames);
		EXPECT_CALL(worker, onEndFrame)
			.Times(numFrames);

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

}

TEST(TFrameWorkerThreadTest, RunMultipleFramesBuffered)
{
	run_multiple_frames_buffered_test<2>();
	run_multiple_frames_buffered_test<3>();
	run_multiple_frames_buffered_test<4>();
}

TEST(TFrameWorkerThreadTest, RunSmartPtrCaptureWork)
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

		// Test for 1000 times to reveal possible threading error
		for(int i = 0; i < 1000; ++i)
		{
			const int numWorksToAdd = i;

			TMockFrameWorker<1, void(void)> worker;
			EXPECT_CALL(worker, onAsyncProcessWork)
				.Times(numWorksToAdd);
			EXPECT_CALL(worker, onBeginFrame)
				.Times(1);
			EXPECT_CALL(worker, onEndFrame)
				.Times(1);

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
