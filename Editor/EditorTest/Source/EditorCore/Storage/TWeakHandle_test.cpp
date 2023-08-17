#include <EditorCore/Storage/TWeakHandle.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <string>

using namespace ph::editor;

TEST(TWeakHandleTest, BasicRequirements)
{
	{
		using IntHandle = TWeakHandle<int>;
		static_assert(std::is_trivially_copyable_v<IntHandle>);
	}
	
	{
		using FloatHandle = TWeakHandle<float, int, int>;
		static_assert(std::is_trivially_copyable_v<FloatHandle>);
	}

	{
		using DoubleHandle = TWeakHandle<double, std::size_t, int>;
		static_assert(std::is_trivially_copyable_v<DoubleHandle>);
	}

	{
		using StringHandle = TWeakHandle<std::string>;
		static_assert(std::is_trivially_copyable_v<StringHandle>);
	}
}

TEST(TWeakHandleTest, Creation)
{
	{
		TWeakHandle<int> handle;
		EXPECT_TRUE(handle.isEmpty());
		EXPECT_FALSE(handle);
	}

	{
		TWeakHandle<float> handle(1, 2);
		EXPECT_FALSE(handle.isEmpty());
		EXPECT_TRUE(handle);
	}

	{
		TWeakHandle<std::string> handle;
		EXPECT_TRUE(handle.isEmpty());
		EXPECT_FALSE(handle);
	}
}

namespace
{

class Base
{};

class Derived : public Base
{};

}

TEST(TWeakHandleTest, Conversion)
{
	// Base to Base
	{
		TWeakHandle<Base> baseHandle(33, 44);
		TWeakHandle<Base> baseHandle2;
		baseHandle2 = baseHandle;
		EXPECT_EQ(baseHandle, baseHandle2);
	}

	// Derived to Base is allowed
	{
		TWeakHandle<Derived> derivedHandle(12, 34);
		TWeakHandle<Base> baseHandle;
		baseHandle = derivedHandle;
		EXPECT_EQ(baseHandle, derivedHandle);
	}
}
