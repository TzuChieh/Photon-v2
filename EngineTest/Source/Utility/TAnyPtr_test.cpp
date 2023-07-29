#include <Utility/TAnyPtr.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <array>
#include <string>

using namespace ph;

TEST(TAnyPtrTest, NullPointer)
{
	{
		AnyConstPtr ptr;
		EXPECT_FALSE(ptr);
	}

	{
		AnyNonConstPtr ptr;
		EXPECT_FALSE(ptr);
	}

	{
		AnyConstPtr ptr(nullptr);
		EXPECT_FALSE(ptr);
	}

	{
		AnyNonConstPtr ptr(nullptr);
		EXPECT_FALSE(ptr);
	}
}

TEST(TAnyPtrTest, PrimitivePointer)
{
	// Null pointer
	{
		int* valuePtr = nullptr;
		AnyConstPtr cptr(valuePtr);
		AnyNonConstPtr ptr(valuePtr);
		EXPECT_FALSE(cptr);
		EXPECT_FALSE(ptr);
	}

	// Integer pointer
	{
		int value = 3;
		AnyConstPtr cptr(&value);
		AnyNonConstPtr ptr(&value);
		EXPECT_TRUE(cptr);
		EXPECT_TRUE(ptr);

		EXPECT_EQ(*(cptr.get<int>()), 3);
		EXPECT_EQ(*(ptr.get<int>()), 3);

		*(ptr.get<int>()) = 10;
		EXPECT_EQ(*(ptr.get<int>()), 10);
	}

	// Floating-point pointer
	{
		float value = -2.2f;
		AnyConstPtr cptr(&value);
		AnyNonConstPtr ptr(&value);
		EXPECT_TRUE(cptr);
		EXPECT_TRUE(ptr);

		EXPECT_EQ(*(cptr.get<float>()), -2.2f);
		EXPECT_EQ(*(ptr.get<float>()), -2.2f);

		*(ptr.get<float>()) = 12345.678f;
		EXPECT_EQ(*(ptr.get<float>()), 12345.678f);
	}

	// Larger integer pointer
	{
		std::uint64_t value = std::numeric_limits<std::uint64_t>::max();
		AnyConstPtr cptr(&value);
		AnyNonConstPtr ptr(&value);
		EXPECT_TRUE(cptr);
		EXPECT_TRUE(ptr);

		EXPECT_EQ(*(cptr.get<std::uint64_t>()), std::numeric_limits<std::uint64_t>::max());
		EXPECT_EQ(*(ptr.get<std::uint64_t>()), std::numeric_limits<std::uint64_t>::max());

		*(ptr.get<std::uint64_t>()) = 33445566778899ull;
		EXPECT_EQ(*(ptr.get<std::uint64_t>()), 33445566778899ull);
	}

	// Larger floating-point pointer
	{
		double value = std::numeric_limits<double>::max();
		AnyConstPtr cptr(&value);
		AnyNonConstPtr ptr(&value);
		EXPECT_TRUE(cptr);
		EXPECT_TRUE(ptr);

		EXPECT_EQ(*(cptr.get<double>()), std::numeric_limits<double>::max());
		EXPECT_EQ(*(ptr.get<double>()), std::numeric_limits<double>::max());

		*(ptr.get<double>()) = -1.1;
		EXPECT_EQ(*(ptr.get<double>()), -1.1);
	}

	// Accessing through wrong type
	{
		char value = 100;
		AnyConstPtr cptr(&value);
		AnyNonConstPtr ptr(&value);
		EXPECT_TRUE(cptr);
		EXPECT_TRUE(ptr);

		EXPECT_FALSE(cptr.get<float>());
		EXPECT_FALSE(cptr.get<int>());
		EXPECT_FALSE(cptr.get<std::int64_t>());
		EXPECT_FALSE(cptr.get<double>());
		EXPECT_FALSE(cptr.get<unsigned char>());

		EXPECT_FALSE(cptr.get<float>());
		EXPECT_FALSE(cptr.get<int>());
		EXPECT_FALSE(cptr.get<std::int64_t>());
		EXPECT_FALSE(cptr.get<double>());
		EXPECT_FALSE(cptr.get<unsigned char>());

		ASSERT_TRUE(cptr.get<char>());
		ASSERT_TRUE(ptr.get<char>());
		EXPECT_EQ(*(cptr.get<char>()), 100);
		EXPECT_EQ(*(ptr.get<char>()), 100);
	}
}

TEST(TAnyPtrTest, ObjectPointer)
{
	// Pointer to std::string
	{
		std::string str = "hello any pointer";
		AnyConstPtr cptr(&str);
		EXPECT_STREQ("hello any pointer", cptr.get<std::string>()->c_str());

		AnyNonConstPtr ptr(&str);
		*(ptr.get<std::string>()) = "hello this is modified";
		EXPECT_STREQ("hello this is modified", ptr.get<std::string>()->c_str());
		EXPECT_STREQ("hello this is modified", cptr.get<std::string>()->c_str());
	}

	// Pointer to large array
	{
		std::array<int, 128> values;
		for(int i = 0; i < values.size(); ++i)
		{
			values[i] = i;
		}

		AnyConstPtr cptr(&values);
		for(int i = 0; i < values.size(); ++i)
		{
			const std::array<int, 128>* valuesPtr = cptr;
			EXPECT_EQ((*valuesPtr)[i], i);
		}

		// Pointer to int pointer
		AnyConstPtr intPtr(values.data());
		for(int i = 0; i < values.size(); ++i)
		{
			const int* valuesPtr = intPtr;
			EXPECT_EQ(valuesPtr[i], i);
		}
	}
}
