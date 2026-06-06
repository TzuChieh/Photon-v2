#include <Common/Utility/CommandLineArguments.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ph;

TEST(CommandLineArgumentsTest, RetrieveString)
{
	// Simulated command line with synthetic arguments
	char arg0[] = "my_app.exe";
	char arg1[] = "--arg-1";
	char arg2[] = "value-1";
	char arg3[] = "--arg-2";
	char arg4[] = "22";
	char* argv[] = {arg0, arg1, arg2, arg3, arg4};
	int argc = 5;

	CommandLineArguments args(argc, argv);

	// The program name should be correctly captured
	EXPECT_EQ(args.getProgramName(), "my_app.exe");

	// Initially, the arguments list is not empty
	EXPECT_FALSE(args.isEmpty());

	// Retrieving the first argument "--arg-1"
	EXPECT_EQ(args.retrieveString(), "--arg-1");

	// Retrieving "value-1" as a string
	EXPECT_EQ(args.retrieveString(), "value-1");

	// The remaining arguments should be "--arg-2" and "22"
	EXPECT_FALSE(args.isEmpty());

	// Retrieving "--arg-2"
	EXPECT_EQ(args.retrieveString(), "--arg-2");

	// Retrieving "22"
	EXPECT_EQ(args.retrieveString(), "22");

	// Retrieving from empty list returns the default value
	EXPECT_EQ(args.retrieveString("default"), "default");
}

TEST(CommandLineArgumentsTest, RetrieveInt)
{
	// Simulated command line with generic numeric values
	char arg0[] = "numeric_app.exe";
	char arg1[] = "-999";
	char arg2[] = "123";
	char* argv[] = {arg0, arg1, arg2};
	int argc = 3;

	CommandLineArguments args(argc, argv);

	// Retrieve and parse as signed integer
	EXPECT_EQ(args.retrieveInt<int>(), -999);

	// Retrieve and parse as unsigned integer
	EXPECT_EQ(args.retrieveInt<unsigned int>(), 123);

	// Retrieving from empty list returns the default value
	EXPECT_EQ(args.retrieveInt<int>(555), 555);
}

TEST(CommandLineArgumentsTest, RetrieveFloat)
{
	// Simulated command line with generic numeric values
	char arg0[] = "numeric_app.exe";
	char arg1[] = "123.456";
	char arg2[] = "-0.001";
	char* argv[] = {arg0, arg1, arg2};
	int argc = 3;

	CommandLineArguments args(argc, argv);

	// Retrieve and parse as float
	EXPECT_NEAR(args.retrieveFloat<float>(), 123.456f, 1e-3f);

	// Retrieve and parse as double
	EXPECT_DOUBLE_EQ(args.retrieveFloat<double>(), -0.001);

	// Retrieving from empty list returns the default value
	EXPECT_EQ(args.retrieveFloat<float>(7.89f), 7.89f);
}

TEST(CommandLineArgumentsTest, RetrieveOptionArguments)
{
	// Simulated command line with various generic options
	char arg0[] = "synthetic_app.exe";
	char arg1[] = "-a";
	char arg2[] = "alpha";
	char arg3[] = "--beta";
	char arg4[] = "1024";
	char arg5[] = "768";
	char arg6[] = "-gamma";
	char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5, arg6};
	int argc = 7;

	CommandLineArguments args(argc, argv);

	// Retrieving arguments for the option "-a"
	std::vector<std::string> aArgs = args.retrieveOptionArguments("-a");
	ASSERT_EQ(aArgs.size(), 1);
	EXPECT_EQ(aArgs[0], "alpha");

	// Retrieving arguments for the option "--beta"
	std::vector<std::string> betaArgs = args.retrieveOptionArguments("--beta");
	ASSERT_EQ(betaArgs.size(), 2);
	EXPECT_EQ(betaArgs[0], "1024");
	EXPECT_EQ(betaArgs[1], "768");

	// Option "-gamma" has no arguments, should return an empty vector
	std::vector<std::string> gammaArgs = args.retrieveOptionArguments("-gamma");
	EXPECT_TRUE(gammaArgs.empty());
	
	// Retrieving a non-existent option should also return an empty vector
	EXPECT_TRUE(args.retrieveOptionArguments("-delta").empty());
}

TEST(CommandLineArgumentsTest, RetrieveStrings)
{
	// Simulated command line with generic marked ranges
	char arg0[] = "range_app.exe";
	char arg1[] = "<begin>";
	char arg2[] = "val-a";
	char arg3[] = "val-b";
	char arg4[] = "<end>";
	char arg5[] = "trailing";
	char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};
	int argc = 6;

	CommandLineArguments args(argc, argv);

	// Retrieve items between "<begin>" and "<end>", inclusive
	std::vector<std::string> range = args.retrieveStrings("<begin>", "<end>");
	ASSERT_EQ(range.size(), 4);
	EXPECT_EQ(range[0], "<begin>");
	EXPECT_EQ(range[1], "val-a");
	EXPECT_EQ(range[2], "val-b");
	EXPECT_EQ(range[3], "<end>");

	// The remaining argument should be "trailing"
	EXPECT_EQ(args.retrieveString(), "trailing");
	EXPECT_TRUE(args.isEmpty());
}

TEST(CommandLineArgumentsTest, RetrieveCommaSeparatedStrings)
{
	char arg0[] = "list_app.exe";
	char arg1[] = "beauty,variance,albedo";
	char arg2[] = "  first , second,third  ";
	char arg3[] = "a,,c";
	char arg4[] = ",,only-value,,";
	char arg5[] = "a,b,   ";
	char* argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};
	int argc = 6;

	CommandLineArguments args(argc, argv);

	std::vector<std::string> values = args.retrieveCommaSeparatedStrings();
	ASSERT_EQ(values.size(), 3);
	EXPECT_EQ(values[0], "beauty");
	EXPECT_EQ(values[1], "variance");
	EXPECT_EQ(values[2], "albedo");

	values = args.retrieveCommaSeparatedStrings();
	ASSERT_EQ(values.size(), 3);
	EXPECT_EQ(values[0], "first");
	EXPECT_EQ(values[1], "second");
	EXPECT_EQ(values[2], "third");

	values = args.retrieveCommaSeparatedStrings();
	ASSERT_EQ(values.size(), 3);
	EXPECT_EQ(values[0], "a");
	EXPECT_TRUE(values[1].empty());
	EXPECT_EQ(values[2], "c");

	values = args.retrieveCommaSeparatedStrings();
	ASSERT_EQ(values.size(), 4);
	EXPECT_TRUE(values[0].empty());
	EXPECT_TRUE(values[1].empty());
	EXPECT_EQ(values[2], "only-value");
	EXPECT_TRUE(values[3].empty());

	values = args.retrieveCommaSeparatedStrings();
	ASSERT_EQ(values.size(), 2);
	EXPECT_EQ(values[0], "a");
	EXPECT_EQ(values[1], "b");
	EXPECT_TRUE(args.isEmpty());
}

TEST(CommandLineArgumentsTest, RetrieveCommaSeparatedStringsUsesDefault)
{
	char arg0[] = "list_app.exe";
	char* argv[] = {arg0};
	int argc = 1;

	CommandLineArguments args(argc, argv);

	const std::vector<std::string> values = args.retrieveCommaSeparatedStrings("a,b");
	ASSERT_EQ(values.size(), 2);
	EXPECT_EQ(values[0], "a");
	EXPECT_EQ(values[1], "b");
}

TEST(CommandLineArgumentsTest, RetrieveCommaSeparatedStringsEmptyInput)
{
	char arg0[] = "list_app.exe";
	char arg1[] = "";
	char* argv[] = {arg0, arg1};
	int argc = 2;

	CommandLineArguments args(argc, argv);

	EXPECT_TRUE(args.retrieveCommaSeparatedStrings().empty());
	EXPECT_TRUE(args.retrieveCommaSeparatedStrings("").empty());
}
