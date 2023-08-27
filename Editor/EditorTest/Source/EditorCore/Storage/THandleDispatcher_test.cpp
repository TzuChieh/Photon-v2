#include <EditorCore/Storage/THandleDispatcher.h>
#include <EditorCore/Storage/TWeakHandle.h>

#include <gtest/gtest.h>

#include <vector>
#include <utility>

using namespace ph::editor;

TEST(THandleDispatcherTest, Dispatchment)
{
	// Dispatch and return
	{
		using Handle = TWeakHandle<int>;

		constexpr int numHandles = 1000;

		THandleDispatcher<Handle> dispatcher;
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

	// Copy from a dispatcher then continue to dispatch
	{
		using Handle = TWeakHandle<int>;

		constexpr int numHandles = 1000;

		THandleDispatcher<Handle> dispatcher;
		std::vector<Handle> handles(numHandles * 2);
		for(int i = 0; i < numHandles; ++i)
		{
			auto handle = dispatcher.dispatchOne();
			handles[handle.getIndex()] = handle;
		}

		THandleDispatcher<Handle> copiedDispatcher = dispatcher;
		for(int i = 0; i < numHandles; ++i)
		{
			auto handle = copiedDispatcher.dispatchOne();

			// Handles dispatched from a moved dispatcher never duplicate with previous handles
			EXPECT_FALSE(handles[handle.getIndex()]);
			handles[handle.getIndex()] = handle;
		}
	}

	// Move from a dispatcher then continue to dispatch
	{
		using Handle = TWeakHandle<int>;

		constexpr int numHandles = 1000;

		THandleDispatcher<Handle> dispatcher;
		std::vector<Handle> handles(numHandles * 2);
		for(int i = 0; i < numHandles; ++i)
		{
			auto handle = dispatcher.dispatchOne();
			handles[handle.getIndex()] = handle;
		}

		THandleDispatcher<Handle> movedDispatcher = std::move(dispatcher);
		for(int i = 0; i < numHandles; ++i)
		{
			auto handle = movedDispatcher.dispatchOne();

			// Handles dispatched from a copied dispatcher never duplicate with previous handles
			EXPECT_FALSE(handles[handle.getIndex()]);
			handles[handle.getIndex()] = handle;
		}
	}
}
