#include <Common/Log/Logger.h>
#include <Common/Log/ELogLevel.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

using namespace ph;

TEST(LoggerTest, LogWithDefaultLevel)
{
	Logger logger;

	std::string loggedMessage;
	ELogLevel loggedLevel = ELogLevel::Debug;// initialize with a different level

	// Add a handler to capture the log message and its level
	logger.addLogHandler(
		[&loggedMessage, &loggedLevel](const ELogLevel level, const std::string_view message)
		{
			loggedLevel = level;
			loggedMessage = message;
		});

	// Log a basic message; default level should be ELogLevel::Note
	const std::string message = "test-default-level";
	logger.log(message);

	EXPECT_EQ(loggedLevel, ELogLevel::Note);

	// The log string should contain the message
	EXPECT_NE(loggedMessage.find(message), std::string::npos);
}

TEST(LoggerTest, LogWithSpecifiedLevel)
{
	Logger logger;

	std::vector<std::pair<ELogLevel, std::string>> loggedData;

	// Add a handler to collect all log entries
	logger.addLogHandler(
		[&loggedData](const ELogLevel level, const std::string_view message)
		{
			loggedData.emplace_back(level, std::string(message));
		});

	// Log with different levels and verify they are captured correctly
	logger.log(ELogLevel::Debug, "debug-msg");
	logger.log(ELogLevel::Warning, "warning-msg");
	logger.log(ELogLevel::Error, "error-msg");

	ASSERT_EQ(loggedData.size(), 3);

	EXPECT_EQ(loggedData[0].first, ELogLevel::Debug);
	EXPECT_NE(loggedData[0].second.find("debug-msg"), std::string::npos);

	EXPECT_EQ(loggedData[1].first, ELogLevel::Warning);
	EXPECT_NE(loggedData[1].second.find("warning-msg"), std::string::npos);

	EXPECT_EQ(loggedData[2].first, ELogLevel::Error);
	EXPECT_NE(loggedData[2].second.find("error-msg"), std::string::npos);
}

TEST(LoggerTest, LogWithNameAndSpecifiedLevel)
{
	Logger logger;

	std::string loggedMessage;

	// Capture the full formatted log string
	logger.addLogHandler(
		[&loggedMessage](const ELogLevel level, const std::string_view message)
		{
			loggedMessage = message;
		});

	// Log with a specific group name
	const std::string groupName = "MyGroup";
	const std::string message = "my-message";
	logger.log(groupName, ELogLevel::Note, message);

	// The log string should contain both the group name and the message
	EXPECT_NE(loggedMessage.find(groupName), std::string::npos);
	EXPECT_NE(loggedMessage.find(message), std::string::npos);
}

TEST(LoggerTest, MultipleLogHandlers)
{
	Logger logger;

	int callCountA = 0;
	int callCountB = 0;

	// Add the first handler
	logger.addLogHandler(
		[&callCountA](const ELogLevel level, const std::string_view message)
		{
			callCountA++;
		});

	// Add the second handler
	logger.addLogHandler(
		[&callCountB](const ELogLevel level, const std::string_view message)
		{
			callCountB++;
		});

	// Log a message; both handlers should be triggered
	logger.log("trigger-all-handlers");

	EXPECT_EQ(callCountA, 1);
	EXPECT_EQ(callCountB, 1);
}
