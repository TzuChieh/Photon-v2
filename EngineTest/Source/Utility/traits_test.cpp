#include <Utility/traits.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(TraitsTest, CanMultiply)
{
	EXPECT_TRUE((ph::CanMultiply<int, int, int>{}));
	EXPECT_TRUE((ph::CanMultiply<float, float, float>{}));
	EXPECT_FALSE((ph::CanMultiply<float, float, std::string>{}));

	class A {};

	class B {};

	class C
	{
	public: 
		B operator * (A b) { return B(); }
		C operator * (B b) { return C(); }
	};

	class D : public C {};

	EXPECT_FALSE((ph::CanMultiply<A, B, C       >{}));
	EXPECT_FALSE((ph::CanMultiply<A, B, const C&>{}));

	EXPECT_FALSE((ph::CanMultiply<A, B, D       >{}));
	EXPECT_FALSE((ph::CanMultiply<A, B, const D&>{}));

	EXPECT_TRUE((ph::CanMultiply<C, A, B       >{}));
	EXPECT_TRUE((ph::CanMultiply<C, A, const B&>{}));

	EXPECT_TRUE((ph::CanMultiply<C, B, C       >{}));
	EXPECT_TRUE((ph::CanMultiply<C, B, const C&>{}));

	EXPECT_TRUE((ph::CanMultiply<D, A, B       >{}));
	EXPECT_TRUE((ph::CanMultiply<D, A, const B&>{}));

	EXPECT_TRUE((ph::CanMultiply<D, B, C       >{}));
	EXPECT_TRUE((ph::CanMultiply<D, B, const C&>{}));

	EXPECT_FALSE((ph::CanMultiply<D, B, D       >{}));
	EXPECT_FALSE((ph::CanMultiply<D, B, const D&>{}));
}

TEST(TraitsTest, CanAdd)
{
	EXPECT_TRUE((ph::CanAdd<int, int, int>{}));
	EXPECT_TRUE((ph::CanAdd<float, float, float>{}));
	EXPECT_FALSE((ph::CanAdd<float, float, std::string>{}));

	class A {};

	class B {};

	class C
	{
	public: 
		B operator + (A b) { return B(); }
		C operator + (B b) { return C(); }
	};

	class D : public C {};

	EXPECT_FALSE((ph::CanAdd<A, B, C       >{}));
	EXPECT_FALSE((ph::CanAdd<A, B, const C&>{}));

	EXPECT_FALSE((ph::CanAdd<A, B, D       >{}));
	EXPECT_FALSE((ph::CanAdd<A, B, const D&>{}));

	EXPECT_TRUE((ph::CanAdd<C, A, B       >{}));
	EXPECT_TRUE((ph::CanAdd<C, A, const B&>{}));

	EXPECT_TRUE((ph::CanAdd<C, B, C       >{}));
	EXPECT_TRUE((ph::CanAdd<C, B, const C&>{}));

	EXPECT_TRUE((ph::CanAdd<D, A, B       >{}));
	EXPECT_TRUE((ph::CanAdd<D, A, const B&>{}));

	EXPECT_TRUE((ph::CanAdd<D, B, C       >{}));
	EXPECT_TRUE((ph::CanAdd<D, B, const C&>{}));

	EXPECT_FALSE((ph::CanAdd<D, B, D       >{}));
	EXPECT_FALSE((ph::CanAdd<D, B, const D&>{}));
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
