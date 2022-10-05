#include <EditorCore/Thread/TFrameWorkerThread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace ph::editor;

//using testing::AtLeast;

namespace
{

template<std::size_t NUM_BUFFERED_FRAMES, typename WorkSignature>
class TMockFrameWorker : public TFrameWorkerThread<NUM_BUFFERED_FRAMES, WorkSignature>
{
	using Base = TFrameWorkerThread<NUM_BUFFERED_FRAMES, WorkSignature>;
	using Work = Base::Work;

public:
	MOCK_METHOD(void, onAsyncProcessWork, (const Work& work), (override));
	MOCK_METHOD(void, onBeginFrame, (std::size_t frameNumber), (override));
	MOCK_METHOD(void, onEndFrame, (), (override));
};

}// end anonymous namespace

TEST(TFrameWorkerThreadTest, RunSingleFrame)
{
	// Smallest possible work duration is running a single frame then exit

	// No work and no buffer
	{
		// Test for 1000 times to reveal possible threading error
		for(int i = 0; i < 1000; ++i)
		{
			TMockFrameWorker<0, void(void)> worker;
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

	// Single simple work
	{

	}
}
