#include <Utility/TFunction.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{

struct SimpleGetSet
{
	int x = 0;

	int get() const
	{
		return x;
	}

	void set(int inX)
	{
		x = inX;
	}
};

float get_squared_number(float num)
{
	return num * num;
}

int get_sum(int a, int b, int c)
{
	return a + b + c;
}

struct Adder
{
	int operator () (int x, int y)
	{
		return x + y;
	}
};

}

TEST(TFunctionTest, Traits)
{
	{
		using NumSquarer = TFunction<float(float)>;

		static_assert(NumSquarer::TIsFreeFunction<&get_squared_number>{} == true);
	}

	{
		using IntGetter = TFunction<int()>;

		static_assert(IntGetter::TIsFreeFunction<&SimpleGetSet::get>{} == false);
		static_assert(IntGetter::TIsConstCallableMethod<&SimpleGetSet::get, SimpleGetSet>{} == true);
		static_assert(IntGetter::TIsNonConstCallableMethod<&SimpleGetSet::get, SimpleGetSet>{} == true);
	}

	{
		using IntSetter = TFunction<void(int)>;

		static_assert(IntSetter::TIsFreeFunction<&SimpleGetSet::set>{} == false);
		static_assert(IntSetter::TIsConstCallableMethod<&SimpleGetSet::set, SimpleGetSet>{} == false);
		static_assert(IntSetter::TIsNonConstCallableMethod<&SimpleGetSet::set, SimpleGetSet>{} == true);
	}

	{
		using IntAdder = TFunction<int(int, int)>;

		auto lambdaFunc = [](int, int) -> int {};
		
		int val;
		auto lambdaFunc2 = [val](int, int) -> int {};

		static_assert(IntAdder::TIsEmptyFunctor<Adder>{} == true);
		static_assert(IntAdder::TIsEmptyFunctor<SimpleGetSet>{} == false);
		static_assert(IntAdder::TIsEmptyFunctor<decltype(lambdaFunc)>{} == true);
		static_assert(IntAdder::TIsEmptyFunctor<decltype(lambdaFunc2)>{} == false);
	}
}

TEST(TFunctionTest, States)
{
	{
		auto func = TFunction<int(int, int, int)>();
		ASSERT_FALSE(func);

		EXPECT_THROW(func(1, 2, 3), UninitializedObjectException);

		func.set<&get_sum>();

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		EXPECT_NO_THROW(func(1, 2, 3));

		func.unset();

		// Two forms for checking validity
		ASSERT_FALSE(func);
		ASSERT_FALSE(func.isValid());

		EXPECT_THROW(func(1, 2, 3), UninitializedObjectException);
	}
}

TEST(TFunctionTest, CallFreeFunction)
{
	{
		auto func = TFunction<float(float)>().set<&get_squared_number>();

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		EXPECT_EQ(func(2.0f), 4.0f);
		EXPECT_EQ(func(-3.0f), 9.0f);
		EXPECT_EQ(func(123.456f), 123.456f * 123.456f);
	}

	{
		auto func = TFunction<int(int, int, int)>().set<&get_sum>();

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		EXPECT_EQ(func(1, 1, 1), 3);
		EXPECT_EQ(func(1, 2, 3), 6);
		EXPECT_EQ(func(-2, 2, 4), 4);
		EXPECT_EQ(func(-123, 456, 789), -123 + 456 + 789);
	}
}

TEST(TFunctionTest, CallMethod)
{
	{
		SimpleGetSet obj;
		auto func = TFunction<int()>().set<&SimpleGetSet::get>(&obj);

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		EXPECT_EQ(func(), 0);

		obj.x = 3;
		EXPECT_EQ(func(), 3);

		obj.x = -4;
		EXPECT_EQ(func(), -4);
	}

	{
		SimpleGetSet obj;
		auto func = TFunction<void(int)>().set<&SimpleGetSet::set>(&obj);

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		func(6);
		EXPECT_EQ(obj.x, 6);

		func(-999);
		EXPECT_EQ(obj.x, -999);
	}

	// Calling method of const object
	{
		const SimpleGetSet obj{777};
		auto func = TFunction<int()>().set<&SimpleGetSet::get>(&obj);

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		EXPECT_EQ(func(), 777);
	}
}

TEST(TFunctionTest, CallEmptyFunctor)
{
	// Call empty struct's operator ()
	{
		auto func = TFunction<int(int, int)>().set<Adder>();
		
		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		EXPECT_EQ(func(3, 6), 9);
	}

	// Call lambda without capture
	{
		auto func = TFunction<int(int, int)>().set([](int x, int y){ return x * y; });

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		EXPECT_EQ(func(-123, 10), -1230);
	}
}
