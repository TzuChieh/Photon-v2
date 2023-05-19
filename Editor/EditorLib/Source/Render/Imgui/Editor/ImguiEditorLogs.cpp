#include "Render/Imgui/Editor/ImguiEditorLogs.h"

#include <Utility/Concurrent/TAtomicQueue.h>

namespace ph::editor
{

namespace
{

struct LogMessage
{
	ELogLevel level;
	std::string text;
};

struct LogStorage
{
	TAtomicQueue<LogMessage> logs;
	TAtomicQueue<LogMessage> freeLogs;
};

inline LogStorage& LOG_STORAGE()
{
	static LogStorage storage;
	return storage;
}

}// end anonymous namespace

void ImguiEditorLogs::engineLogHook(const ELogLevel logLevel, const std::string_view logString)
{
	LogStorage& storage = LOG_STORAGE();

	LogMessage message;
	storage.freeLogs.tryDequeue(&message);

	message.level = logLevel;
	message.text = logString;

	storage.logs.enqueue(std::move(message));
}

}// end namespace ph::editor
