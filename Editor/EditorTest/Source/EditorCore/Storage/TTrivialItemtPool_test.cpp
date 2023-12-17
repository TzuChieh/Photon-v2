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

class SimpleClass2
{
public:
	int x = -777;
	float y = 2.0f;
	double z = 3.0;
};

static_assert(std::is_trivially_copyable_v<SimpleClass>);
static_assert(std::is_trivially_copyable_v<SimpleClass2>);

}

TEST(TTrivialItemPoolTest, Creation)
{
	{
		TTrivialItemPool<int> pool;
		EXPECT_EQ(pool.numItems(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}

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

TEST(TTrivialItemPoolTest, RemovedItemHasDefaultValue)
{
	// Using `SimpleClass2`, which has non-zero default values
	using Pool = TTrivialItemPool<SimpleClass2>;
	using Handle = Pool::HandleType;

	constexpr int numItems = 1000;

	Pool pool;

	// Create dummy items
	std::vector<Handle> handles;
	for(int i = 0; i < numItems; ++i)
	{
		// Make unique internal state, actual value doesn't matter,
		// just need to be different than default ones
		SimpleClass2 obj;
		obj.x = i * 3;
		obj.y = i * 4.0f;
		obj.z = i * -123.0;

		handles.push_back(pool.add(obj));
	}

	const SimpleClass2 defaultObj;

	// Case 1: removing one by one
	{
		Pool copiedPool = pool;
		for(int i = 0; i < numItems; ++i)
		{
			Handle newHandle = copiedPool.removeAt(handles[i]);
			SimpleClass2* obj = copiedPool.get(newHandle);
			ASSERT_TRUE(obj);

			EXPECT_EQ(obj->x, defaultObj.x);
			EXPECT_EQ(obj->y, defaultObj.y);
			EXPECT_EQ(obj->z, defaultObj.z);
		}
	}

	// Case 2: implicitly removed (non-initialized items)
	{
		Pool copiedPool = pool;

		// Make sure capacity grows larger than `numItems` so we can test whether non-initialized
		// items also have default values. If failed, try to make this condition true again.
		EXPECT_GT(copiedPool.capacity(), numItems);

		for(int i = numItems; i < copiedPool.capacity(); ++i)
		{
			Handle newHandle = copiedPool.dispatchOneHandle();
			SimpleClass2* obj = copiedPool.get(newHandle);
			ASSERT_TRUE(obj);

			EXPECT_EQ(obj->x, defaultObj.x);
			EXPECT_EQ(obj->y, defaultObj.y);
			EXPECT_EQ(obj->z, defaultObj.z);
		}
	}
}

namespace
{

struct WithConst
{
	const int x;

	WithConst()
		: x(-1)
	{}

	WithConst(int x)
		: x(x)
	{}
};

}

TEST(TTrivialItemPoolTest, ItemWithConstQualifier)
{
	// With const member + adding & removing with auto handle management
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;
		using Pool = TTrivialItemPool<WithConst, Dispatcher>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 10000;

		Pool pool;
		std::vector<Handle> handles;
		for(int i = 0; i < numItems; ++i)
		{
			handles.push_back(pool.add(WithConst(i)));
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			WithConst* item = pool.get(handles[i]);
			ASSERT_TRUE(item);

			EXPECT_EQ(item->x, i);
		}

		// Remove every item
		for(int i = 0; i < numItems; ++i)
		{
			pool.remove(handles[i]);
			EXPECT_FALSE(pool.get(handles[i]));
		}
		EXPECT_EQ(pool.numItems(), 0);

		// Add items again, this time with a different value
		handles.clear();
		for(int i = 0; i < numItems; ++i)
		{
			handles.push_back(pool.add(WithConst(i * 2)));
		}

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			WithConst* item = pool.get(handles[i]);
			ASSERT_TRUE(item);

			EXPECT_EQ(item->x, i * 2);
		}
	}

	// Const instances + adding & removing with auto handle management
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;
		using Pool = TTrivialItemPool<const SimpleClass, Dispatcher>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 10000;

		Pool pool;
		std::vector<Handle> handles;
		for(int i = 0; i < numItems; ++i)
		{
			SimpleClass item;
			item.x = i;
			item.y = i * -1.1f;
			item.z = i * 2.2;

			handles.push_back(pool.add(item));
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			const SimpleClass* item = pool.get(handles[i]);
			ASSERT_TRUE(item);

			EXPECT_EQ(item->x, i);
			EXPECT_EQ(item->y, i * -1.1f);
			EXPECT_EQ(item->z, i * 2.2);
		}

		// Remove every item
		for(int i = 0; i < numItems; ++i)
		{
			pool.remove(handles[i]);
			EXPECT_FALSE(pool.get(handles[i]));
		}
		EXPECT_EQ(pool.numItems(), 0);

		// Add items again, this time with a different value
		handles.clear();
		for(int i = 0; i < numItems; ++i)
		{
			SimpleClass item;
			item.x = i;
			item.y = i * 3.3f;
			item.z = i * -4.4;

			handles.push_back(pool.add(item));
		}

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			const SimpleClass* item = pool.get(handles[i]);
			ASSERT_TRUE(item);

			EXPECT_EQ(item->x, i);
			EXPECT_EQ(item->y, i * 3.3f);
			EXPECT_EQ(item->z, i * -4.4);
		}
	}
}
