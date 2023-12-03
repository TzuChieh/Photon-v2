#include <ph_core.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>

int main(int argc, char* argv[])
{
	if(!ph::init_render_engine(ph::EngineInitSettings{}))
	{
		std::cerr << "Photon initialization failed" << std::endl;
		return EXIT_FAILURE;
	}

	// Since Google Mock depends on Google Test, InitGoogleMock() is
	// also responsible for initializing Google Test. Therefore there's
	// no need for calling testing::InitGoogleTest() separately.
	testing::InitGoogleMock(&argc, argv);
	const int testReturnValue = RUN_ALL_TESTS();

	if(!ph::exit_render_engine())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return testReturnValue;
}
