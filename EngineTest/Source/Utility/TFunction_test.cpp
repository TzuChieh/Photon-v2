#include <Utility/TFunction.h>

#include <gtest/gtest.h>

using namespace ph;

namespace
{

struct SimpleGetter
{
	int x = 0;

	int get() const
	{
		return x;
	}
};

}

TEST(TFunctionTest, Traits)
{
	{
		using IntGetter = TFunction<int()>;

		static_assert(IntGetter::TIsFreeFunction<&SimpleGetter::get>{} == false);
		static_assert(IntGetter::TIsConstMethod<&SimpleGetter::get, SimpleGetter>{} == true);
		static_assert(IntGetter::TIsNonConstMethod<&SimpleGetter::get, SimpleGetter>{} == false);
	}
}
