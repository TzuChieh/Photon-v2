#include <Engine/Utility/traits.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <type_traits>

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

TEST(TriatsTest, IsDerived)
{
	// A class type is its own derived type
	EXPECT_TRUE((ph::CDerived<std::string, std::string>));

	// Primitive types are never related to derivations
	EXPECT_FALSE((ph::CDerived<int, int>));
	EXPECT_FALSE((ph::CDerived<int, float>));
	EXPECT_FALSE((ph::CDerived<double, double>));
}

namespace
{

inline void function_callable()
{}

inline int function_callable2(float x, unsigned long long y)
{
	return 123;
}

struct MethodCallable
{
	inline void nonConstFunc(int x)
	{}

	inline void constFunc(int x) const
	{}
};

struct FunctorCallable
{
	inline float operator () (int x, int y, double z) const
	{
		return 3.14f;
	}
};

inline auto lambdaCallable = [](char ch) -> void
{};

}// end namespace

TEST(TriatsTest, CallableTraits)
{
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(function_callable)>::ReturnType, void>));
	EXPECT_EQ((std::tuple_size_v<ph::TCallableTraits<decltype(function_callable)>::ArgTypes>), 0);

	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(function_callable2)>::ReturnType, int>));
	EXPECT_EQ((std::tuple_size_v<ph::TCallableTraits<decltype(function_callable2)>::ArgTypes>), 2);
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(function_callable2)>::ArgTypeAt<0>, float>));
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(function_callable2)>::ArgTypeAt<1>, unsigned long long>));

	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(&MethodCallable::nonConstFunc)>::ReturnType, void>));
	EXPECT_EQ((std::tuple_size_v<ph::TCallableTraits<decltype(&MethodCallable::nonConstFunc)>::ArgTypes>), 1);
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(&MethodCallable::nonConstFunc)>::ArgTypeAt<0>, int>));

	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(&MethodCallable::constFunc)>::ReturnType, void>));
	EXPECT_EQ((std::tuple_size_v<ph::TCallableTraits<decltype(&MethodCallable::constFunc)>::ArgTypes>), 1);
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(&MethodCallable::constFunc)>::ArgTypeAt<0>, int>));

	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<FunctorCallable>::ReturnType, float>));
	EXPECT_EQ((std::tuple_size_v<ph::TCallableTraits<FunctorCallable>::ArgTypes>), 3);
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<FunctorCallable>::ArgTypeAt<0>, int>));
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<FunctorCallable>::ArgTypeAt<1>, int>));
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<FunctorCallable>::ArgTypeAt<2>, double>));

	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(lambdaCallable)>::ReturnType, void>));
	EXPECT_EQ((std::tuple_size_v<ph::TCallableTraits<decltype(lambdaCallable)>::ArgTypes>), 1);
	EXPECT_TRUE((std::is_same_v<ph::TCallableTraits<decltype(lambdaCallable)>::ArgTypeAt<0>, char>));
}
