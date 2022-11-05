#include <Utility/TFunction.h>

#include <gtest/gtest.h>

#include <array>

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

struct MemberGetter
{
	int x;
	float y;
	double z;

	void operator () (int* out_x, float* out_y, double* out_z) const
	{
		*out_x = x;
		*out_y = y;
		*out_z = z;
	}
};

struct MemberIsRef
{
	float& fRef;

	explicit MemberIsRef(float& value)
		: fRef(value)
	{}

	void operator () () const
	{}
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

	{
		// Without size hint: macro determines the lower bound
		static_assert(sizeof(TFunction<int(int, int)>) >= PH_TFUNCTION_DEFAULT_MIN_SIZE_IN_BYTES);

		// Limit the size to 2 pointers: 
		// 
		// *** NOTE: 
		// This test is relatively strict, e.g., function pointers do not necessarily have
		// a size equal to normal pointers. Consider relaxing it if error occurred.
		// ***
		//
		constexpr auto sizeofTwoPointers = sizeof(void*) * 2;
		static_assert(sizeof(TFunction<int(int, int), sizeofTwoPointers>) == sizeofTwoPointers);
	}

	{
		int smallVar;
		auto smallFunc = [smallVar](){};
		static_assert(TFunction<void(void)>::TCanFitBuffer<decltype(smallFunc)>{} == true);

		std::array<int, 1024> largeVar;
		auto largeFunc = [largeVar](){};
		static_assert(TFunction<void(void)>::TCanFitBuffer<decltype(largeFunc)>{} == false);
	}

	// Store non-empty functor with reference member 
	// Note: `MemberIsRef` still satisfy the standard's "trivially copyable" requirements, 
	//       so does reference-capturing lambda
	{
		using Func = TFunction<void(void)>;

		static_assert(Func::TIsNonEmptyTrivialFunctor<MemberIsRef>{} == true);

		float value;
		auto refCapturingLambda = [&value](void) -> void {};
		static_assert(Func::TIsNonEmptyTrivialFunctor<decltype(refCapturingLambda)>{} == true);
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

	// Use direct-init ctor
	{
		TFunction<int(int, int)> multiplier = [](int x, int y)
		{
			return x * y;
		};

		// Two forms for checking validity
		ASSERT_TRUE(multiplier);
		ASSERT_TRUE(multiplier.isValid());

		EXPECT_EQ(multiplier(5, -50), -250);
	}
}

TEST(TFunctionTest, CallNonEmptyFunctor)
{
	// Call non-empty functor
	{
		auto func = TFunction<void(int*, float*, double*)>().set(MemberGetter{3, 4.0f, 5.0f});

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		int x;
		float y;
		double z;
		func(&x, &y, &z);
		EXPECT_EQ(x, 3);
		EXPECT_EQ(y, 4.0f);
		EXPECT_EQ(z, 5.0);
	}

	// Call capturing lambda
	{
		int x = 6;
		float y = 7.7f;
		double z = 12345.0;

		// Simply get the captured values
		auto func = TFunction<void(int*, float*, double*)>().set(
			[x, y, z](int* out_x, float* out_y, double* out_z)
			{
				*out_x = x;
				*out_y = y;
				*out_z = z;
			});

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		int testX;
		float testY;
		double testZ;
		func(&testX, &testY, &testZ);
		EXPECT_EQ(testX, 6);
		EXPECT_EQ(testY, 7.7f);
		EXPECT_EQ(testZ, 12345.0);
	}

	// Call capturing lambda
	{
		// Start the counter from 10
		int counter = 10;

		auto func = TFunction<void()>().set(
			[&counter]()
			{
				++counter;
			});

		// Two forms for checking validity
		ASSERT_TRUE(func);
		ASSERT_TRUE(func.isValid());

		// Call 1000 times
		for(int i = 0; i < 1000; ++i)
		{
			func();
		}

		// Should be 10 (initial) + 1000 (loop) = 1010
		EXPECT_EQ(counter, 1010);
	}

	// Use direct-init ctor
	{
		int constant1 = 3;
		float constant2 = 1.0f;
		double constant3 = -4.0;
		TFunction<double(int)> multiplier = [constant1, constant2, constant3](int x)
		{
			return constant1 * constant2 * constant3 * x;
		};

		// Two forms for checking validity
		ASSERT_TRUE(multiplier);
		ASSERT_TRUE(multiplier.isValid());

		EXPECT_EQ(multiplier(0), 0.0);
		EXPECT_EQ(multiplier(1), -12.0);
		EXPECT_EQ(multiplier(-1), 12.0);
		EXPECT_EQ(multiplier(5), -60.0);
	}
}
