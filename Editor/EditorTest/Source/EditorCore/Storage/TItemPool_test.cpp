#include <EditorCore/Storage/TItemPool.h>

#include <gtest/gtest.h>

#include <vector>

using namespace ph::editor;

TEST(TItemPoolTest, Creation)
{
	{
		TItemPool<int> pool;
		EXPECT_EQ(pool.numItems(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}

	/*{
		TItemPool<std::vector<double>> pool;
		EXPECT_EQ(pool.numItems(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}*/
}

TEST(TItemPoolTest, AddingItems)
{
	{
		TItemPool<float> pool;

		pool.add(1.1f);
		EXPECT_EQ(pool.numItems(), 1);
		EXPECT_FALSE(pool.isEmpty());

		pool.add(2.2f);
		pool.add(3.3f);
		EXPECT_EQ(pool.numItems(), 3);
	}

	{
		constexpr int numItems = 1000;

		TItemPool<int> pool;
		for(int i = 0; i < numItems; ++i)
		{
			pool.add(i);
		}
		EXPECT_EQ(pool.numItems(), numItems);
	}

	// Trivial item
	{
		using Pool = TItemPool<int>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 1000;

		std::vector<Handle> handles;
		Pool pool;
		for(int i = 0; i < numItems; ++i)
		{
			handles.push_back(pool.add(i));
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		for(int i = 0; i < numItems; ++i)
		{
			int* ptr = pool.get(handles[i]);
			ASSERT_TRUE(pool.get(handles[i]));
			EXPECT_EQ(*ptr, i);
		}

		// Out-of-bound access
		for(int i = numItems; i < numItems * 2; ++i)
		{
			EXPECT_FALSE(pool.get(Handle(i, 0)));
		}
	}

	// Non-trivial item
	//{
	//	using Pool = TItemPool<std::vector<float>>;
	//	using Handle = Pool::HandleType;

	//	constexpr int numItems = 1000;

	//	std::vector<Handle> handles;
	//	Pool pool;
	//	for(int i = 0; i < numItems; ++i)
	//	{
	//		float value = i;
	//		handles.push_back(pool.add({value, value, value}));
	//	}
	//	ASSERT_EQ(pool.numItems(), handles.size());

	//	for(int i = 0; i < numItems; ++i)
	//	{
	//		std::vector<float>* ptr = pool.get(handles[i]);
	//		ASSERT_TRUE(pool.get(handles[i]));
	//		ASSERT_EQ(ptr->size(), 3);
	//		EXPECT_EQ((*ptr)[0], i);
	//		EXPECT_EQ((*ptr)[1], i);
	//		EXPECT_EQ((*ptr)[2], i);
	//	}

	//	// Out-of-bound access
	//	for(int i = numItems; i < numItems * 2; ++i)
	//	{
	//		EXPECT_FALSE(pool.get(Handle(i, 0)));
	//	}
	//}
}

TEST(TItemPoolTest, AddingAndRemovingItems)
{
	// TODO
}

TEST(TItemPoolTest, RangedFor)
{
	// TODO
}
