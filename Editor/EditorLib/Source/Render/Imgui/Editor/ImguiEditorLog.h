#pragma once

#include "Render/Imgui/ImguiEditorPanel.h"

#include <Common/Log/logger_fwd.h>
#include <Common/primitive_type.h>

#include <string>
#include <string_view>
#include <vector>

namespace ph::editor
{

class ImguiEditorLog : public ImguiEditorPanel
{
public:
	explicit ImguiEditorLog(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

	static void engineLogHook(ELogLevel logLevel, std::string_view logString);

private:
	struct LogMessage
	{
		std::string text;
		ELogLevel level;
		// TODO: passed filter?
	};

	void retrieveNewLogs();
	void clearLogs();

	// TODO: filter 1: combobox for log level (toggle)
	// TODO: filter 2: word filter

	static bool tryRetrieveOneLog(LogMessage* out_message);

	std::vector<LogMessage> m_logBuffer;
	std::size_t m_numLogs;
	std::size_t m_numClearedLogs;
	bool m_isAutoScrollEnabled;
};

}// end namespace ph::editor
