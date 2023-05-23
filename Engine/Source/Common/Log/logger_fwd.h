#pragma once

#include <functional>
#include <string_view>

namespace ph
{

enum class ELogLevel;
class Logger;

using LogHandler = std::function<void(ELogLevel logLevel, std::string_view logString)>;

}// end namespace ph
