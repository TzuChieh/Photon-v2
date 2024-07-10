#include <ph_core.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
	// HACK: cd to expected working directory so Photon can properly init (test adapter in
	// VS 17.10.1 is discovering tests in wrong working directory)
	if(std::filesystem::current_path().filename() == "bin")
	{
		std::filesystem::current_path(std::filesystem::current_path().parent_path());
	}

	// Since Google Mock depends on Google Test, InitGoogleMock() is
	// also responsible for initializing Google Test. Therefore there's
	// no need for calling testing::InitGoogleTest() separately.
	testing::InitGoogleMock(&argc, argv);

	if(!ph::init_render_engine())
	{
		std::cerr << "Photon initialization failed" << std::endl;
		return EXIT_FAILURE;
	}

	const int testReturnValue = RUN_ALL_TESTS();

	if(!ph::exit_render_engine())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return testReturnValue;
}
