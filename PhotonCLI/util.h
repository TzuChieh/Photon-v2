#pragma once

#include <ph_core.h>

#include <string>


#define PH_CLI_NAMESPACE ph_cli

#define PH_CLI_NAMESPACE_BEGIN namespace PH_CLI_NAMESPACE {
#define PH_CLI_NAMESPACE_END   }

PH_CLI_NAMESPACE_BEGIN

void save_frame_with_fail_safe(PHuint64 frameId, const std::string& filePath);

PH_CLI_NAMESPACE_END