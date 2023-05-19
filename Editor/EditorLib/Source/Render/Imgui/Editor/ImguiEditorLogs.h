#pragma once

#include <Common/Log/logger_fwd.h>
#include <Utility/IUninstantiable.h>

#include <string_view>

namespace ph::editor
{

class ImguiEditorLogs final : private IUninstantiable
{
public:
	static void engineLogHook(ELogLevel logLevel, std::string_view logString);
	static bool tryPopLog();
};

}// end namespace ph::editor
