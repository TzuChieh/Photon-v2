#pragma once

namespace ph { class EngineInitSettings; }

namespace ph
{

bool init_engine_core(const EngineInitSettings& settings);
bool init_engine_IO_infrastructure(const EngineInitSettings& settings);
bool exit_API_database();

}// end namespace ph
