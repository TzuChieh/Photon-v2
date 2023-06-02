#include "Render/Imgui/Editor/ImguiEditorLog.h"

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <Utility/Concurrent/TAtomicQueue.h>
#include <Utility/utility.h>
#include <Common/Log/ELogLevel.h>

#include <cstddef>
#include <utility>

namespace ph::editor
{

namespace
{

struct QueuedLog
{
	std::string text;
	ELogLevel level;
};

struct LogStorage
{
	TAtomicQueue<QueuedLog> logs;
	TAtomicQueue<QueuedLog> freeLogs;
};

inline LogStorage& LOG_STORAGE()
{
	static LogStorage storage;
	return storage;
}

}// end anonymous namespace

ImguiEditorLog::ImguiEditorLog()
	: m_logBuffer()
	, m_numLogs(0)
	, m_numClearedLogs(0)
	, isAutoScrollEnabled(true)
{
	constexpr std::size_t numReservedLogs = 128;
	constexpr std::size_t numEstimatedLogsPerLine = 128;

	m_logBuffer.resize(numReservedLogs);
	for(LogMessage& message : m_logBuffer)
	{
		message.text.reserve(numEstimatedLogsPerLine);
	}
}

void ImguiEditorLog::buildWindow(const char* title, bool* isOpening)
{
	if(!ImGui::Begin(title, isOpening))
	{
		return ImGui::End();
	}

	if(ImGui::Button("Clear"))
	{
		clearLogs();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &isAutoScrollEnabled);
	ImGui::SameLine();
	const auto numLogs = lossless_cast<int>(m_numLogs);
	const auto numTotalLogs = lossless_cast<int>(m_numLogs + m_numClearedLogs);
	ImGui::Text("Showing %d/%d logs", numLogs, numTotalLogs);
	ImGui::Separator();

	ImGui::BeginChild("##scrolling_child", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	retrieveNewLogs();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGuiListClipper clipper;
	clipper.Begin(numLogs);
	while(clipper.Step())
	{
		for(int logIdx = clipper.DisplayStart; logIdx < clipper.DisplayEnd; ++logIdx)
		{
			const LogMessage& log = m_logBuffer[logIdx];

			switch(log.level)
			{
			case ELogLevel::Debug: 
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.2f, 1.0f, 1.0f)); break;
			case ELogLevel::Warning:
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f)); break;
			case ELogLevel::Error:
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); break;
			default: 
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f)); break;
			}
			ImGui::TextUnformatted(log.text.data(), log.text.data() + log.text.size());
			ImGui::PopStyleColor();
		}
	}
	clipper.End();
	ImGui::PopStyleVar();

	if(isAutoScrollEnabled && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::End();
}

void ImguiEditorLog::retrieveNewLogs()
{
	while(true)
	{
		// Possibly add a new buffer if we run out of them
		PH_ASSERT_LE(m_numLogs, m_logBuffer.size());
		if(m_numLogs == m_logBuffer.size())
		{
			m_logBuffer.push_back(LogMessage());
		}

		PH_ASSERT_LT(m_numLogs, m_logBuffer.size());
		LogMessage& message = m_logBuffer[m_numLogs];
		if(tryRetrieveOneLog(&message))
		{
			++m_numLogs;
		}
		else
		{
			break;
		}
	}
}

void ImguiEditorLog::clearLogs()
{
	m_numClearedLogs += m_numLogs;
	m_numLogs = 0;
}

bool ImguiEditorLog::tryRetrieveOneLog(LogMessage* const out_message)
{
	PH_ASSERT(out_message);

	LogStorage& storage = LOG_STORAGE();

	QueuedLog log;
	if(!storage.logs.tryDequeue(&log))
	{
		return false;
	}

	out_message->text = log.text;
	out_message->level = log.level;

	// Recycle the buffer
	storage.freeLogs.enqueue(std::move(log));

	return true;
}

void ImguiEditorLog::engineLogHook(const ELogLevel logLevel, const std::string_view logString)
{
	LogStorage& storage = LOG_STORAGE();

	QueuedLog log;
	// Get a recycled buffer. 
	// No need to check return value--if failed, `log` can just be the new buffer we need
	storage.freeLogs.tryDequeue(&log);

	log.text = logString;
	log.level = logLevel;

	storage.logs.enqueue(std::move(log));
}

}// end namespace ph::editor