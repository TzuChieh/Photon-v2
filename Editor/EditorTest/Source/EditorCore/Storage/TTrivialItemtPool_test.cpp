#include <EditorCore/Storage/TTrivialItemPool.h>
#include <EditorCore/Storage/TConcurrentHandleDispatcher.h>
#include <EditorCore/Storage/TWeakHandle.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph::editor;

namespace
{

class SimpleClass
{
public:
	int x = 0;
	float y = 0.0f;
	double z = 0.0;
};

static_assert(std::is_trivially_copyable_v<SimpleClass>);

}

TEST(TTrivialItemPoolTest, Creation)
{
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;

		TTrivialItemPool<int, Dispatcher> pool;
		EXPECT_EQ(pool.numItems(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}

	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;

		TTrivialItemPool<SimpleClass, Dispatcher> pool;
		EXPECT_EQ(pool.numItems(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}
}

TEST(TTrivialItemPoolTest, AddingAndRemovingItems)
{
	// Adding & removing with auto handle management
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;
		using Pool = TTrivialItemPool<SimpleClass, Dispatcher>;
		using Handle = Pool::HandleType;
		using HandleDispatcher = TConcurrentHandleDispatcher<Handle>;

		constexpr int numItems = 10000;

		Pool pool;
		std::vector<Handle> handles;
		for(int i = 0; i < numItems; ++i)
		{
			SimpleClass item;
			item.x = i;
			item.y = i * 2.0f;
			item.z = i * 3.0;

			handles.push_back(pool.add(item));
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			SimpleClass* item = pool.get(handles[i]);
			ASSERT_TRUE(item);

			EXPECT_EQ(item->x, i);
			EXPECT_EQ(item->y, i * 2.0f);
			EXPECT_EQ(item->z, i * 3.0);
		}

		// Remove every item
		for(int i = 0; i < numItems; ++i)
		{
			pool.remove(handles[i]);
			EXPECT_FALSE(pool.get(handles[i]));
		}
		EXPECT_EQ(pool.numItems(), 0);
	}

	// Adding & removing with manual handle management
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;
		using Pool = TTrivialItemPool<SimpleClass, Dispatcher>;
		using Handle = Pool::HandleType;
		using HandleDispatcher = TConcurrentHandleDispatcher<Handle>;

		constexpr int numItems = 10000;

		Pool pool;

		// We can create all handles beforehand
		std::vector<Handle> handles;
		for(int i = 0; i < numItems; ++i)
		{
			handles.push_back(pool.dispatchOneHandle());
		}
		ASSERT_EQ(pool.numItems(), 0);

		// Then add items using the created handles
		for(int i = 0; i < numItems; ++i)
		{
			SimpleClass item;
			item.x = i;
			item.y = i * 2.0f;
			item.z = i * 3.0;

			pool.createAt(handles[i], item);
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			SimpleClass* item = pool.get(handles[i]);
			ASSERT_TRUE(item);

			EXPECT_EQ(item->x, i);
			EXPECT_EQ(item->y, i * 2.0f);
			EXPECT_EQ(item->z, i * 3.0);
		}

		// We can remove all items without returning handles
		for(int i = 0; i < numItems; ++i)
		{
			Handle oldHandle = handles[i];
			Handle newHandle = pool.removeAt(oldHandle);
			handles[i] = newHandle;

			EXPECT_FALSE(pool.isFresh(oldHandle));
			EXPECT_TRUE(pool.isFresh(newHandle));
		}
		ASSERT_EQ(pool.numItems(), 0);

		// Actually return the updated handles
		for(int i = 0; i < numItems; ++i)
		{
			pool.returnOneHandle(handles[i]);
			EXPECT_TRUE(pool.isFresh(handles[i]));
		}
		EXPECT_EQ(pool.numItems(), 0);
	}
}
