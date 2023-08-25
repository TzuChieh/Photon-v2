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
		TGraphicsObjectPool<int> pool;
		EXPECT_EQ(pool.numObjects(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}

	{
		TGraphicsObjectPool<SimpleClass> pool;
		EXPECT_EQ(pool.numObjects(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}
}

TEST(TGraphicsObjectPoolTest, AddingObjects)
{
	// Simply adding
	{

	}
}

TEST(TGraphicsObjectPoolTest, AddingAndRemovingObjects)
{
	{
		using Pool = TGraphicsObjectPool<SimpleClass>;
		using Handle = Pool::HandleType;
		using HandleDispatcher = TConcurrentHandleDispatcher<Handle>;

		constexpr int numObjs = 10000;

		Pool pool;
		HandleDispatcher dispatcher;
		std::vector<Handle> handles;
		for(int i = 0; i < numObjs; ++i)
		{
			Handle handle = dispatcher.dispatchOne();

			SimpleClass obj;
			obj.x = i;
			obj.y = i * 2.0f;
			obj.z = i * 3.0;

			pool.createAt(handle, obj);
			handles.push_back(handle);
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
			pool.removeAt(handles[i]);
			EXPECT_FALSE(pool.get(handles[i]));
		}
		EXPECT_EQ(pool.numObjects(), 0);
	}
}
