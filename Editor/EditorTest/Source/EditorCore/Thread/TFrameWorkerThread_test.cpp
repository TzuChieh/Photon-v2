#include <EditorCore/Thread/TFrameWorkerThread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <memory>
#include <algorithm>
#include <thread>
#include <vector>
#include <atomic>
#include <type_traits>

#if !GTEST_IS_THREADSAFE 
	#error "TFrameWorkerThreadTest requires googletest to be thread safe."
#endif

using namespace ph::editor;

using testing::Between;

namespace
{

template<std::size_t NUM_BUFFERS, typename WorkSignature>
class TMockFrameWorker : public TFrameWorkerThread<NUM_BUFFERS, WorkSignature>
{
public:
	using Base = TFrameWorkerThread<NUM_BUFFERS, WorkSignature>;
	using Work = Base::Work;

public:
	using Base::Base;

	MOCK_METHOD(void, onAsyncProcessWork, (const Work& work), (override));
	MOCK_METHOD(void, onBeginFrame, (), (override));
	MOCK_METHOD(void, onEndFrame, (), (override));
};

template<std::size_t NUM_BUFFERS>
class TSimpleFrameWorker : public TFrameWorkerThread<NUM_BUFFERS, void()>
{
public:
	using Base = TFrameWorkerThread<NUM_BUFFERS, void()>;
	using Work = Base::Work;

public:
	using Base::Base;

	inline void onAsyncProcessWork(const Work& work) override
	{
		work();
	}
};

}

TEST(TFrameWorkerThreadTest, RunSingleFrameUnbuffered)
{
	// Smallest possible work duration is running a single frame then exit

	// No work and no buffer
	{
		// Test for 100 times to reveal possible threading error
		for(int i = 0; i < 100; ++i)
		{
			TMockFrameWorker<1, void(void)> worker;
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

	// Add empty works in allowed places
	{
		TMockFrameWorker<1, void(void)> worker;
		EXPECT_CALL(worker, onAsyncProcessWork)
			.Times(5);
		EXPECT_CALL(worker, onBeginFrame)
			.Times(1);
		EXPECT_CALL(worker, onEndFrame)
			.Times(1);

		worker.startWorker();
		worker.beginFrame();
		worker.addWork([](){});// #1
		worker.addWork([](){});// #2
		worker.addWork([](){});// #3
		worker.requestWorkerStop();
		worker.addWork([](){});// #4
		worker.addWork([](){});// #5
		worker.endFrame();
		worker.waitForWorkerToStop();
	}

	// Incrementally add small works
	{
		// Test for 100 times to reveal possible threading error
		for(int i = 0; i < 100; ++i)
		{
			const int numWorksToAdd = i;

			TMockFrameWorker<1, int(int)> worker;
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

			TMockFrameWorker<1, int(int)> worker;
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

namespace
{

template<std::size_t NUM_BUFFERS>
inline void run_multiple_frames_test(const bool shouldFlushBufferBeforeStop)
{
	constexpr std::size_t MAX_FRAMES = 20;

	// Test from 1 frame to `MAX_FRAMES` frames to reveal possible threading error
	for(int numFrames = 1; numFrames <= MAX_FRAMES; ++numFrames)
	{
		const int numSmallWorksToAdd = numFrames * 5;
		const int numLargeWorksToAdd = numFrames * 2;

		TMockFrameWorker<NUM_BUFFERS, void(int, int, int)> worker(shouldFlushBufferBeforeStop);

		int maxSkippedFrames = 0;
		if(!shouldFlushBufferBeforeStop)
		{
			// For N buffered frames, we may process every work (#frames * #works); or at most, skipped
			// N frames (all buffering frames including the current frame do not get to be processed, 
			// works processed = (#frames - N) * #works)
			maxSkippedFrames = std::min(numFrames, static_cast<int>(NUM_BUFFERS));
		}

		EXPECT_CALL(worker, onAsyncProcessWork)
			.Times(Between(
				(numFrames - maxSkippedFrames) * (numSmallWorksToAdd + numLargeWorksToAdd),
				(numFrames) * (numSmallWorksToAdd + numLargeWorksToAdd)));

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

}

TEST(TFrameWorkerThreadTest, RunMultipleFramesBuffered)
{
	// Flush buffer before stop
	run_multiple_frames_test<2>(true);
	run_multiple_frames_test<3>(true);
	run_multiple_frames_test<4>(true);

	// Not flushing buffer before stop
	run_multiple_frames_test<2>(false);
	run_multiple_frames_test<3>(false);
	run_multiple_frames_test<4>(false);
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

		// Test for 100 times to reveal possible threading error
		for(int i = 0; i < 100; ++i)
		{
			const int numWorksToAdd = i;

			TMockFrameWorker<1, void(void)> worker;
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

TEST(TFrameWorkerThreadTest, WorkObjectDestruct)
{
	// Mixing shared ptr and unique ptr
	auto testFunc = [](const bool shouldFlushBufferBeforeStop)
	{
		for(int i = 1; i < 10; ++i)
		{
			const int numWorksPerFrame = i;
			const int numFramesToRun = i;
			const int numAdditionalWorks = i;

			// Note: this is a buffered worker
			TSimpleFrameWorker<3> worker(shouldFlushBufferBeforeStop);
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

					// Adding works after requesting stop should be allowed
					for(std::size_t wi = 0; wi < numAdditionalWorks; ++wi)
					{
						worker.addWork(
							[ptr = std::unique_ptr<int, decltype(ptrDeleter)>(new int(), ptrDeleter)]()
							{
								ASSERT_TRUE(ptr);
							});
					}
				}

				worker.endFrame();
			}
			
			worker.waitForWorkerToStop();

			// Though the worker is buffered, all works should still be destructed to ensure correctness
			const auto numTotalWorks = numWorksPerFrame * numFramesToRun + numAdditionalWorks;
			EXPECT_EQ(deleteCount, numTotalWorks);
		}
	};

	// Flush buffer before stop
	testFunc(true);

	// Not flushing buffer before stop
	testFunc(false);
}

namespace
{

template<std::size_t NUM_BUFFERS>
inline void multiple_small_work_producers_test(
	const std::size_t numProducers, 
	const std::size_t numWorksPerProducer,
	const std::size_t numFramesToRun)
{
	std::atomic_uint32_t counter = 0;

	using Worker = TSimpleFrameWorker<NUM_BUFFERS>;
	Worker worker;

	worker.startWorker();
	for(int fi = 0; fi < numFramesToRun; ++fi)
	{
		worker.beginFrame();

		std::vector<std::thread> producers(numProducers);
		for(int pi = 0; pi < numProducers; ++pi)
		{
			producers[pi] = std::thread([&worker, &counter, numWorksPerProducer]()
			{
				for(int wi = 0; wi < numWorksPerProducer; ++wi)
				{
					// Explicitly instantiate a worker's work type to ensure storing `smallWork`
					// does not involve the use of arena
					typename Worker::Work smallWork = 
						[&counter]()
						{
							counter.fetch_add(1, std::memory_order_relaxed);
						};

					worker.addWork(smallWork);
				}
			});
		}

		for(auto& producer : producers)
		{
			producer.join();
		}

		// Request stop on last frame
		if(fi == numFramesToRun - 1)
		{
			worker.requestWorkerStop();
		}
		
		worker.endFrame();
	}

	worker.waitForWorkerToStop();
	EXPECT_EQ(
		counter.load(std::memory_order_relaxed), 
		numProducers * numWorksPerProducer * numFramesToRun);
}

}

TEST(TFrameWorkerThreadTest, MultipleSmallWorkProducers)
{
	// Baseline: unbuffered, 1 producer, 10 works/producer, 10 frames
	multiple_small_work_producers_test<1>(1, 10, 10);

	// Baseline: unbuffered, 2 producers, 10 works/producer, 10 frames
	multiple_small_work_producers_test<1>(2, 10, 10);

	// Baseline: unbuffered, 4 producers, 10 works/producer, 10 frames
	multiple_small_work_producers_test<1>(4, 10, 10);

	// Small work set: double buffered, 4 producers, 100 works/producer, 20 frames
	multiple_small_work_producers_test<2>(4, 100, 20);

	// Medium work set: double buffered, 8 producers, 100 works/producer, 21 frames
	multiple_small_work_producers_test<2>(8, 100, 21);

	// Medium work set: triple buffered, 8 producers, 100 works/producer, 22 frames
	multiple_small_work_producers_test<3>(8, 100, 22);

	// Medium work set: quintuple buffered, 8 producers, 100 works/producer, 23 frames
	multiple_small_work_producers_test<5>(8, 100, 23);

	// Larger work set: triple buffered, 8 producers, 1000 works/producer, 50 frames
	multiple_small_work_producers_test<3>(8, 1000, 50);

	// Larger work set: triple buffered, 16 producers, 1000 works/producer, 51 frames
	multiple_small_work_producers_test<3>(16, 1000, 51);

	// Larger work set: quintuple buffered, 20 producers, 1000 works/producer, 123 frames
	multiple_small_work_producers_test<5>(20, 1000, 123);
}

namespace
{

template<std::size_t NUM_BUFFERS>
inline void multiple_large_work_producers_test(
	const std::size_t numProducers, 
	const std::size_t numWorksPerProducer,
	const std::size_t numFramesToRun)
{
	std::atomic_uint32_t counter = 0;

	using Worker = TSimpleFrameWorker<NUM_BUFFERS>;
	Worker worker;

	worker.startWorker();
	for(int fi = 0; fi < numFramesToRun; ++fi)
	{
		worker.beginFrame();

		std::vector<std::thread> producers(numProducers);
		for(int pi = 0; pi < numProducers; ++pi)
		{
			producers[pi] = std::thread([&worker, &counter, numWorksPerProducer]()
			{
				for(int wi = 0; wi < numWorksPerProducer; ++wi)
				{
					worker.addWork(
						[&counter, 
						 nonTrivial = std::make_unique<double>(wi), 
						 largeObj = std::array<int, 128>{}]()
						{
							counter.fetch_add(1, std::memory_order_relaxed);
						});
				}
			});
		}

		for(auto& producer : producers)
		{
			producer.join();
		}

		// Request stop on last frame
		if(fi == numFramesToRun - 1)
		{
			worker.requestWorkerStop();
		}
		
		worker.endFrame();
	}

	worker.waitForWorkerToStop();
	EXPECT_EQ(
		counter.load(std::memory_order_relaxed), 
		numProducers * numWorksPerProducer * numFramesToRun);
}

}

TEST(TFrameWorkerThreadTest, MultipleLargeWorkProducers)
{
	// Baseline: unbuffered, 1 producer, 10 works/producer, 10 frames
	multiple_large_work_producers_test<1>(1, 10, 10);

	// Baseline: unbuffered, 2 producers, 10 works/producer, 10 frames
	multiple_large_work_producers_test<1>(2, 10, 10);

	// Baseline: unbuffered, 4 producers, 10 works/producer, 10 frames
	multiple_large_work_producers_test<1>(4, 10, 10);

	// Small work set: double buffered, 4 producers, 100 works/producer, 20 frames
	multiple_large_work_producers_test<2>(4, 100, 20);

	// Medium work set: double buffered, 8 producers, 100 works/producer, 21 frames
	multiple_large_work_producers_test<2>(8, 100, 21);

	// Medium work set: triple buffered, 8 producers, 100 works/producer, 22 frames
	multiple_large_work_producers_test<3>(8, 100, 22);

	// Medium work set: quintuple buffered, 8 producers, 100 works/producer, 23 frames
	multiple_large_work_producers_test<5>(8, 100, 23);

	// Larger work set: triple buffered, 8 producers, 1000 works/producer, 50 frames
	multiple_large_work_producers_test<3>(8, 1000, 50);

	// Larger work set: triple buffered, 16 producers, 1000 works/producer, 51 frames
	multiple_large_work_producers_test<3>(16, 1000, 51);

	// Larger work set: quintuple buffered, 20 producers, 1000 works/producer, 123 frames
	multiple_large_work_producers_test<5>(20, 1000, 123);
}
