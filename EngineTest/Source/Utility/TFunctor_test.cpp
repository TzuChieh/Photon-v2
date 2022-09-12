#include <Utility/TFunctor.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TFunctorTest, Traits)
{

}

TEST(TFunctorTest, States)
{
	{
		auto func = TFunctor<int(int)>();
	}
}
