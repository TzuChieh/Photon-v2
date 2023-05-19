#pragma once

#include <Common/Log/logger_fwd.h>
#include <Common/primitive_type.h>

#include <string>
#include <string_view>
#include <vector>

namespace ph::editor
{

class ImguiEditorLog final
{
public:
	ImguiEditorLog();

	void buildWindow(const char* title, bool* isOpening = nullptr);

	static void engineLogHook(ELogLevel logLevel, std::string_view logString);

private:
	struct LogMessage
	{
		std::string text;
		ELogLevel level;
		// TODO: passed filter?
	};

	void retrieveNewLogs();
	static bool tryRetrieveOneLog(LogMessage* out_message);

	std::vector<LogMessage> m_logBuffer;
	std::size_t m_numLogs;
	bool isAutoScrollEnabled;
};

}// end namespace ph::editor
