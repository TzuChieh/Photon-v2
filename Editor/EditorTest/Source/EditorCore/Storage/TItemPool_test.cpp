#include <EditorCore/Storage/TItemPool.h>
#include <EditorCore/Storage/TWeakHandle.h>

#include <gtest/gtest.h>

#include <vector>
#include <memory>
#include <utility>

using namespace ph::editor;

namespace
{

class Base
{};

class Derived : public Base
{};

template<typename Pool, typename Handle>
concept CValidHandleForPool = requires (Pool pool, Handle handle)
{
	pool.get(handle);
	std::as_const(pool).get(handle);
	pool.remove(handle);
};

}

TEST(TItemPoolTest, BasicRequirements)
{
	// Accessing derived object using base handle is valid
	{
		using Pool = TItemPool<Derived>;
		using BaseHandle = TWeakHandle<Base>;
		using DerivedHandle = TWeakHandle<Derived>;

		static_assert(CValidHandleForPool<Pool, BaseHandle> == true);
		static_assert(CValidHandleForPool<Pool, DerivedHandle> == true);
	}
	
	// Accessing base object using derived handle is **invalid**
	{
		using Pool = TItemPool<Base>;
		using BaseHandle = TWeakHandle<Base>;
		using DerivedHandle = TWeakHandle<Derived>;

		static_assert(CValidHandleForPool<Pool, BaseHandle> == true);
		static_assert(CValidHandleForPool<Pool, DerivedHandle> == false);
	}
}

TEST(TItemPoolTest, Creation)
{
	{
		TItemPool<int> pool;
		EXPECT_EQ(pool.numItems(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}

	{
		TItemPool<std::vector<double>> pool;
		EXPECT_EQ(pool.numItems(), 0);
		EXPECT_EQ(pool.numFreeSpace(), 0);
		EXPECT_TRUE(pool.isEmpty());
	}
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
	{
		using Pool = TItemPool<std::vector<float>>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 1000;

		std::vector<Handle> handles;
		Pool pool;
		for(int i = 0; i < numItems; ++i)
		{
			auto value = static_cast<float>(i);
			handles.push_back(pool.add({value, value, value}));
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		for(int i = 0; i < numItems; ++i)
		{
			std::vector<float>* ptr = pool.get(handles[i]);
			ASSERT_TRUE(pool.get(handles[i]));
			ASSERT_EQ(ptr->size(), 3);
			EXPECT_EQ((*ptr)[0], i);
			EXPECT_EQ((*ptr)[1], i);
			EXPECT_EQ((*ptr)[2], i);
		}

		// Out-of-bound access
		for(int i = numItems; i < numItems * 2; ++i)
		{
			EXPECT_FALSE(pool.get(Handle(i, 0)));
		}
	}
}

namespace
{

struct DtorCounter
{
	int& count;

	DtorCounter(int& count)
		: count(count)
	{}

	~DtorCounter()
	{
		++count;
	}
};

}

TEST(TItemPoolTest, AddingAndRemovingItems)
{
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

		// Remove every item
		for(int i = 0; i < numItems; ++i)
		{
			int* ptr = pool.get(handles[i]);
			ASSERT_TRUE(pool.get(handles[i]));
			EXPECT_EQ(*ptr, i);

			pool.remove(handles[i]);
			EXPECT_FALSE(pool.get(handles[i]));
		}
		EXPECT_EQ(pool.numItems(), 0);
	}

	// Non-trivial item
	{
		using Pool = TItemPool<std::unique_ptr<DtorCounter>>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 1000;

		std::vector<Handle> handles;
		Pool pool;
		int counter = 0;
		for(int i = 0; i < numItems; ++i)
		{
			handles.push_back(pool.add(std::make_unique<DtorCounter>(counter)));
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		// Remove every item
		for(int i = 0; i < numItems; ++i)
		{
			EXPECT_TRUE(pool.get(handles[i]));
			pool.remove(handles[i]);
			EXPECT_FALSE(pool.get(handles[i]));
		}
		EXPECT_EQ(pool.numItems(), 0);

		// Number of destructor calls
		EXPECT_EQ(counter, numItems);
	}
}

TEST(TItemPoolTest, RangedFor)
{
	// Basic read-only looping
	{
		using Pool = TItemPool<std::unique_ptr<double>>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 1000;

		Pool pool;
		for(int i = 0; i < numItems; ++i)
		{
			auto value = static_cast<double>(i);
			pool.add(std::make_unique<double>(value));
		}
		ASSERT_EQ(pool.numItems(), numItems);

		// Forward looping
		{
			int i = 0;
			for(std::unique_ptr<double>& item : pool)
			{
				auto value = static_cast<double>(i);
				EXPECT_EQ(*item, value);
				++i;
			}
		}

		// Backward looping
		{
			auto iter = pool.end();
			int i = numItems - 1;
			while(iter != pool.begin())
			{
				--iter;
				auto value = static_cast<double>(i);
				EXPECT_EQ(**iter, value);
				--i;
			}
		}

		// Mixed forward & backward advancements
		{
			auto iter = pool.begin();
			std::advance(iter, 3);// at 3
			EXPECT_EQ(**iter, 3.0);
			std::advance(iter, -3);// at 0
			EXPECT_EQ(**iter, 0.0);
			std::advance(iter, 123);// at 123
			EXPECT_EQ(**iter, 123.0);
			std::advance(iter, -50);// at 73
			EXPECT_EQ(**iter, 73.0);
			std::advance(iter, -50);// at 23
			EXPECT_EQ(**iter, 23.0);
			std::advance(iter, 500);// at 523
			EXPECT_EQ(**iter, 523.0);
			std::advance(iter, -523);// at 0
			EXPECT_EQ(**iter, 0.0);
			std::advance(iter, 999);// at 999
			EXPECT_EQ(**iter, 999.0);
			std::advance(iter, 1);// at 1000 (end)
			EXPECT_TRUE(iter == pool.end());
		}
	}
}
