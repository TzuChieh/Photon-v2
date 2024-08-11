#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char* argv[])
{
	// Since Google Mock depends on Google Test, `testing::InitGoogleMock()` is
	// also responsible for initializing Google Test. Therefore there's no need
	// for calling `testing::InitGoogleTest()` separately.
	testing::InitGoogleMock(&argc, argv);

	return RUN_ALL_TESTS();
}
