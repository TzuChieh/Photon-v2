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
	public: 
		B operator * (A b) { return B(); }
		C operator * (B b) { return C(); }
	};

	class D : public C {};

	EXPECT_FALSE((ph::has_multiply_operator<A, B, C       >{}));
	EXPECT_FALSE((ph::has_multiply_operator<A, B, const C&>{}));

	EXPECT_FALSE((ph::has_multiply_operator<A, B, D       >{}));
	EXPECT_FALSE((ph::has_multiply_operator<A, B, const D&>{}));

	EXPECT_TRUE((ph::has_multiply_operator<C, A, B       >{}));
	EXPECT_TRUE((ph::has_multiply_operator<C, A, const B&>{}));

	EXPECT_TRUE((ph::has_multiply_operator<C, B, C       >{}));
	EXPECT_TRUE((ph::has_multiply_operator<C, B, const C&>{}));

	EXPECT_TRUE((ph::has_multiply_operator<D, A, B       >{}));
	EXPECT_TRUE((ph::has_multiply_operator<D, A, const B&>{}));

	EXPECT_TRUE((ph::has_multiply_operator<D, B, C       >{}));
	EXPECT_TRUE((ph::has_multiply_operator<D, B, const C&>{}));

	EXPECT_FALSE((ph::has_multiply_operator<D, B, D       >{}));
	EXPECT_FALSE((ph::has_multiply_operator<D, B, const D&>{}));
}