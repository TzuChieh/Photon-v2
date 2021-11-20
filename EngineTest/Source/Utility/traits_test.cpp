#include <Utility/traits.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(TraitsTest, CanMultiply)
{
	EXPECT_TRUE((ph::CCanMultiply<int, int, int>));
	EXPECT_TRUE((ph::CCanMultiply<float, float, float>));

	class A {};

	class B {};

	class C
	{
	public: 
		B operator * (A b) { return B(); }
		C operator * (B b) { return C(); }
	};

	class D : public C {};

	EXPECT_FALSE((ph::CCanMultiply<A, B, C>));
	EXPECT_FALSE((ph::CCanMultiply<A, B, const C&>));

	EXPECT_FALSE((ph::CCanMultiply<A, B, D>));
	EXPECT_FALSE((ph::CCanMultiply<A, B, const D&>));

	EXPECT_TRUE((ph::CCanMultiply<C, A, B>));
	EXPECT_TRUE((ph::CCanMultiply<C, A, B&>));
	EXPECT_FALSE((ph::CCanMultiply<C, A, const B&>));// cannot assign to const ref

	EXPECT_TRUE((ph::CCanMultiply<C, B, C>));
	EXPECT_TRUE((ph::CCanMultiply<C, B, C&>));
	EXPECT_FALSE((ph::CCanMultiply<C, B, const C&>));// cannot assign to const ref

	EXPECT_TRUE((ph::CCanMultiply<D, A, B>));
	EXPECT_TRUE((ph::CCanMultiply<D, A, B&>));
	EXPECT_FALSE((ph::CCanMultiply<D, A, const B&>));// cannot assign to const ref

	EXPECT_TRUE((ph::CCanMultiply<D, B, C>));
	EXPECT_TRUE((ph::CCanMultiply<D, B, C&>));
	EXPECT_FALSE((ph::CCanMultiply<D, B, const C&>));// cannot assign to const ref

	// Cannot convert D * B back to D
	EXPECT_FALSE((ph::CCanMultiply<D, B, D>));
	EXPECT_FALSE((ph::CCanMultiply<D, B, D&>));
	EXPECT_FALSE((ph::CCanMultiply<D, B, const D&>));
}

TEST(TraitsTest, CanAdd)
{
	EXPECT_TRUE((ph::CCanAdd<int, int, int>));
	EXPECT_TRUE((ph::CCanAdd<float, float, float>));

	class A {};

	class B {};

	class C
	{
	public: 
		B operator + (A b) { return B(); }
		C operator + (B b) { return C(); }
	};

	class D : public C {};

	EXPECT_FALSE((ph::CCanAdd<A, B, C>));
	EXPECT_FALSE((ph::CCanAdd<A, B, const C&>));

	EXPECT_FALSE((ph::CCanAdd<A, B, D>));
	EXPECT_FALSE((ph::CCanAdd<A, B, const D&>));

	EXPECT_TRUE((ph::CCanAdd<C, A, B>));
	EXPECT_TRUE((ph::CCanAdd<C, A, B&>));
	EXPECT_FALSE((ph::CCanAdd<C, A, const B&>));// cannot assign to const ref

	EXPECT_TRUE((ph::CCanAdd<C, B, C>));
	EXPECT_TRUE((ph::CCanAdd<C, B, C&>));
	EXPECT_FALSE((ph::CCanAdd<C, B, const C&>));// cannot assign to const ref

	EXPECT_TRUE((ph::CCanAdd<D, A, B>));
	EXPECT_TRUE((ph::CCanAdd<D, A, B&>));
	EXPECT_FALSE((ph::CCanAdd<D, A, const B&>));// cannot assign to const ref

	EXPECT_TRUE((ph::CCanAdd<D, B, C>));
	EXPECT_TRUE((ph::CCanAdd<D, B, C&>));
	EXPECT_FALSE((ph::CCanAdd<D, B, const C&>));// cannot assign to const ref

	// Cannot convert D + B back to D
	EXPECT_FALSE((ph::CCanAdd<D, B, D>));
	EXPECT_FALSE((ph::CCanAdd<D, B, D&>));
	EXPECT_FALSE((ph::CCanAdd<D, B, const D&>));
}

TEST(TraitsTest, IsBuildable)
{
	EXPECT_TRUE((ph::IsBuildable<int, int>{}));
	EXPECT_TRUE((ph::IsBuildable<float, float>{}));
	EXPECT_TRUE((ph::IsBuildable<unsigned char, int>{}));
	EXPECT_TRUE((ph::IsBuildable<float, unsigned int>{}));
	EXPECT_TRUE((ph::IsBuildable<int, double>{}));

	EXPECT_FALSE((ph::IsBuildable<int, std::string()>{}));
	EXPECT_FALSE((ph::IsBuildable<std::string(), std::vector<float>()>{}));
}
