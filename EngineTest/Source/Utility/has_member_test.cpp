#include <Utility/has_member.h>

#include <gtest/gtest.h>

TEST(HasMemberTest, HasMultiplyOperator)
{
	EXPECT_TRUE((ph::has_multiply_operator<int, int, int>{}));
	EXPECT_TRUE((ph::has_multiply_operator<float, float, float>{}));

	class A {};
	class B {};
	class C
	{
	public: B operator * (A b) { return B(); }
	};

	EXPECT_FALSE((ph::has_multiply_operator<A, B, C>{}));
	EXPECT_TRUE((ph::has_multiply_operator<C, A, B>{}));
}