#include <EditorCore/Storage/TItemPool.h>
#include <EditorCore/Storage/TWeakHandle.h>
#include <EditorCore/Storage/TStrongHandle.h>
#include <EditorCore/Storage/TConcurrentHandleDispatcher.h>

#include <gtest/gtest.h>

#include <vector>
#include <memory>
#include <utility>
#include <string>

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
		using BaseHandle = TWeakHandle<Base>;
		using DerivedHandle = TWeakHandle<Derived>;
		using Pool = TItemPool<Derived>;

		static_assert(CValidHandleForPool<Pool, BaseHandle> == true);
		static_assert(CValidHandleForPool<Pool, DerivedHandle> == true);
	}
	
	// Accessing base object using derived handle is **invalid**
	{
		using BaseHandle = TWeakHandle<Base>;
		using DerivedHandle = TWeakHandle<Derived>;
		using Pool = TItemPool<Base>;

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

TEST(TItemPoolTest, CopyAndMove)
{
	// Copy and move with trivial item type
	{
		using Pool = TItemPool<int>;
		constexpr int numItems = 1000;

		Pool pool;
		for(int i = 0; i < numItems; ++i)
		{
			pool.add(i);
		}
		EXPECT_EQ(pool.numItems(), numItems);

		// Copy
		{
			Pool copiedPool = pool;
			EXPECT_EQ(copiedPool.numItems(), numItems);

			int i = 0;
			for(int value : copiedPool)
			{
				EXPECT_EQ(value, i);
				++i;
			}
			EXPECT_EQ(i, numItems);
		}

		// Move
		{
			Pool movedPool;
			{
				Pool movedFromPool = pool;
				movedPool = std::move(movedFromPool);

				// Empty after being moved from
				EXPECT_EQ(movedFromPool.numItems(), 0);

				// `movedPool` was empty, `movedFromPool` should give all its free space to `movedPool`
				// and now has no free space
				EXPECT_EQ(movedFromPool.numFreeSpace(), 0);
			}
			EXPECT_EQ(movedPool.numItems(), numItems);

			int i = 0;
			for(int value : movedPool)
			{
				EXPECT_EQ(value, i);
				++i;
			}
			EXPECT_EQ(i, numItems);
		}
	}

	// Move from a pool and continue to dispatch (trivial item type)
	{
		using Pool = TItemPool<int>;
		using Handle = typename Pool::HandleType;

		constexpr int numItems = 1000;

		Pool pool;
		std::vector<Handle> handles(numItems * 2);
		for(int i = 0; i < numItems; ++i)
		{
			auto handle = pool.add(i);
			handles[i] = handle;
		}
		EXPECT_EQ(pool.numItems(), numItems);

		Pool movedPool = std::move(pool);
		EXPECT_EQ(movedPool.numItems(), numItems);

		// Original handles should still be usable
		for(int i = 0; i < numItems; ++i)
		{
			EXPECT_TRUE(handles[i]);

			int* ptr = movedPool.get(handles[i]);
			ASSERT_TRUE(ptr);
			EXPECT_EQ(*ptr, i);
		}

		// Continue to add another batch of `numItems` items
		for(int i = numItems; i < numItems * 2; ++i)
		{
			auto handle = movedPool.add(i);
			handles[i] = handle;
		}

		// Check all handles are valid
		for(int i = 0; i < numItems * 2; ++i)
		{
			EXPECT_TRUE(handles[i]);

			int* ptr = movedPool.get(handles[i]);
			ASSERT_TRUE(ptr);
			EXPECT_EQ(*ptr, i);
		}
	}
}

TEST(TItemPoolTest, StrongHandleAccess)
{
	// Basic read only
	{
		using Pool = TItemPool<int>;
		using Handle = TStrongHandle<int>;

		constexpr int numItems = 1000;

		std::vector<Handle> handles;
		Pool pool;
		for(int i = 0; i < numItems; ++i)
		{
			auto weakHandle = pool.add(i);
			auto strongHandle = pool.getStrong(weakHandle);

			EXPECT_TRUE(strongHandle);
			handles.push_back(strongHandle);
		}

		for(int i = 0; i < numItems; ++i)
		{
			ASSERT_TRUE(handles[i].viewItem());
			EXPECT_EQ(*(handles[i].viewItem()), i);
		}
	}

	// Basic write & changing pool items
	{
		using Pool = TItemPool<int>;
		using Handle = TStrongHandle<int>;

		constexpr int numItems = 1000;

		std::vector<Handle> handles;
		Pool pool;
		for(int i = 0; i < numItems; ++i)
		{
			auto weakHandle = pool.add(i);
			auto strongHandle = pool.getStrong(weakHandle);

			EXPECT_TRUE(strongHandle);
			handles.push_back(strongHandle);
		}

		// Make all items 2x
		for(int i = 0; i < numItems; ++i)
		{
			ASSERT_TRUE(handles[i].accessItem());
			EXPECT_EQ(*(handles[i].accessItem()), i);

			*(handles[i].accessItem()) = i * 2;
		}

		for(int i = 0; i < numItems; ++i)
		{
			ASSERT_TRUE(handles[i].accessItem());
			EXPECT_EQ(*(handles[i].accessItem()), i * 2);
		}

		// Remove the first item
		pool.remove(handles.front().getWeak());
		EXPECT_EQ(pool.numItems(), numItems - 1);
		EXPECT_TRUE(handles.front());
		EXPECT_FALSE(handles.front().viewItem());
		EXPECT_FALSE(handles.front().accessItem());

		// Remove all remaining items
		pool.clear();
		for(int i = 0; i < numItems; ++i)
		{
			EXPECT_FALSE(handles[i].viewItem());
			EXPECT_FALSE(handles[i].accessItem());
		}
	}
}

TEST(TItemPoolTest, StorageSpaceReuse)
{
	// Trivial item
	{
		using Pool = TItemPool<int>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 1000;

		Pool pool;
		std::vector<Handle> handles;
		for(int i = 0; i < numItems; ++i)
		{
			handles.push_back(pool.add(i));
		}
		ASSERT_EQ(pool.numItems(), handles.size());
		EXPECT_EQ(pool.numItems(), numItems);
		EXPECT_GE(pool.capacity(), numItems);

		const auto oldCapacity = pool.capacity();

		// Remove every item one by one
		for(int i = 0; i < numItems; ++i)
		{
			pool.remove(handles[i]);
		}
		EXPECT_EQ(pool.numItems(), 0);

		// Add all of them back again, capacity should not change
		for(int i = 0; i < numItems; ++i)
		{
			handles[i] = pool.add(i);
		}
		EXPECT_EQ(pool.numItems(), numItems);
		EXPECT_EQ(pool.capacity(), oldCapacity);

		// Remove every item by clear
		pool.clear();
		EXPECT_EQ(pool.numItems(), 0);

		// Add all of them back again, capacity should not change
		for(int i = 0; i < numItems; ++i)
		{
			handles[i] = pool.add(i);
		}
		EXPECT_EQ(pool.numItems(), numItems);
		EXPECT_EQ(pool.capacity(), oldCapacity);
	}
}

TEST(TItemPoolTest, RecognizeUninitializedItem)
{
	// Trivial item (single one)
	{
		using Pool = TItemPool<int>;
		using Handle = Pool::HandleType;

		Pool pool;

		// Manual handle management
		Handle handle = pool.dispatchOneHandle();

		// The item is not created yet, cannot access it
		EXPECT_FALSE(pool.get(handle));

		pool.createAt(handle, 123);

		// Now should be accessible
		ASSERT_TRUE(pool.get(handle));
		EXPECT_EQ(*(pool.get(handle)), 123);

		pool.remove(handle);

		EXPECT_FALSE(pool.get(handle));
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

TEST(TItemPoolTest, ItemWithConstQualifier)
{
	// With const member + adding & removing with auto handle management
	{
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<WithConst>>;
		using Pool = TItemPool<WithConst, Dispatcher>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 1000;

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
		using Dispatcher = TConcurrentHandleDispatcher<TWeakHandle<const std::string>>;
		using Pool = TItemPool<const std::string, Dispatcher>;
		using Handle = Pool::HandleType;

		constexpr int numItems = 1000;

		Pool pool;
		std::vector<Handle> handles;
		for(int i = 0; i < numItems; ++i)
		{
			handles.push_back(pool.add(std::to_string(i)));
		}
		ASSERT_EQ(pool.numItems(), handles.size());

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			const std::string* str = pool.get(handles[i]);
			ASSERT_TRUE(str);

			EXPECT_STREQ(str->c_str(), std::to_string(i).c_str());
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
			handles.push_back(pool.add(std::to_string(i * 2)));
		}

		// Check every item is intactly stored
		for(int i = 0; i < numItems; ++i)
		{
			const std::string* str = pool.get(handles[i]);
			ASSERT_TRUE(str);

			EXPECT_STREQ(str->c_str(), std::to_string(i * 2).c_str());
		}
	}
}
