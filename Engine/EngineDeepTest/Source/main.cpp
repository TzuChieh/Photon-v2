#include <Engine/ph_core.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char* argv[])
{
	// Since Google Mock depends on Google Test, InitGoogleMock() is
	// also responsible for initializing Google Test. Therefore there's
	// no need for calling testing::InitGoogleTest() separately.
	testing::InitGoogleMock(&argc, argv);

	if(!ph::init_render_engine())
	{
		return EXIT_FAILURE;
	}

	const int testReturnValue = RUN_ALL_TESTS();

	if(!ph::exit_render_engine())
	{
		return EXIT_FAILURE;
	}

	return testReturnValue;
}
