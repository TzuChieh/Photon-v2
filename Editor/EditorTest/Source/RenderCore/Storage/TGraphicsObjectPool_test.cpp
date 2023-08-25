#include <RenderCore/Storage/TGraphicsObjectPool.h>
#include <RenderCore/Storage/TConcurrentHandleDispatcher.h>

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

TEST(TGraphicsObjectPoolTest, Creation)
{
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;

		TGraphicsObjectPool<int, Dispatcher> pool;
		EXPECT_EQ(pool.numObjects(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}

	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;

		TGraphicsObjectPool<SimpleClass, Dispatcher> pool;
		EXPECT_EQ(pool.numObjects(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}
}

TEST(TGraphicsObjectPoolTest, AddingAndRemovingObjects)
{
	// Adding & removing with auto handle management
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;
		using Pool = TGraphicsObjectPool<SimpleClass, Dispatcher>;
		using Handle = Pool::HandleType;
		using HandleDispatcher = TConcurrentHandleDispatcher<Handle>;

		constexpr int numObjs = 10000;

		Pool pool;
		std::vector<Handle> handles;
		for(int i = 0; i < numObjs; ++i)
		{
			SimpleClass obj;
			obj.x = i;
			obj.y = i * 2.0f;
			obj.z = i * 3.0;

			handles.push_back(pool.add(obj));
		}
		ASSERT_EQ(pool.numObjects(), handles.size());

		// Check every object is intactly stored
		for(int i = 0; i < numObjs; ++i)
		{
			SimpleClass* obj = pool.get(handles[i]);
			ASSERT_TRUE(obj);

			EXPECT_EQ(obj->x, i);
			EXPECT_EQ(obj->y, i * 2.0f);
			EXPECT_EQ(obj->z, i * 3.0);
		}

		// Remove every object
		for(int i = 0; i < numObjs; ++i)
		{
			pool.remove(handles[i]);
			EXPECT_FALSE(pool.get(handles[i]));
		}
		EXPECT_EQ(pool.numObjects(), 0);
	}

	// Adding & removing with manual handle management
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<int>>;
		using Pool = TGraphicsObjectPool<SimpleClass, Dispatcher>;
		using Handle = Pool::HandleType;
		using HandleDispatcher = TConcurrentHandleDispatcher<Handle>;

		constexpr int numObjs = 10000;

		Pool pool;

		// We can create all handles beforehand
		std::vector<Handle> handles;
		for(int i = 0; i < numObjs; ++i)
		{
			handles.push_back(pool.dispatchOneHandle());
		}
		ASSERT_EQ(pool.numObjects(), 0);

		// Then add objects using the created handles
		for(int i = 0; i < numObjs; ++i)
		{
			SimpleClass obj;
			obj.x = i;
			obj.y = i * 2.0f;
			obj.z = i * 3.0;

			pool.createAt(handles[i], obj);
		}
		ASSERT_EQ(pool.numObjects(), handles.size());

		// Check every object is intactly stored
		for(int i = 0; i < numObjs; ++i)
		{
			SimpleClass* obj = pool.get(handles[i]);
			ASSERT_TRUE(obj);

			EXPECT_EQ(obj->x, i);
			EXPECT_EQ(obj->y, i * 2.0f);
			EXPECT_EQ(obj->z, i * 3.0);
		}

		// We can remove all objects without returning handles
		for(int i = 0; i < numObjs; ++i)
		{
			Handle oldHandle = handles[i];
			Handle newHandle = pool.removeAt(oldHandle);
			handles[i] = newHandle;

			EXPECT_FALSE(pool.isFresh(oldHandle));
			EXPECT_TRUE(pool.isFresh(newHandle));
		}
		ASSERT_EQ(pool.numObjects(), 0);

		// Actually return the updated handles
		for(int i = 0; i < numObjs; ++i)
		{
			pool.returnOneHandle(handles[i]);
			EXPECT_TRUE(pool.isFresh(handles[i]));
		}
		EXPECT_EQ(pool.numObjects(), 0);
	}
}
