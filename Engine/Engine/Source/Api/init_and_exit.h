#pragma once

namespace ph { class EngineInitSettings; }

namespace ph
{

bool init_engine_core(const EngineInitSettings& settings);
bool init_engine_IO_infrastructure(const EngineInitSettings& settings);
void after_engine_init(const EngineInitSettings& settings);
void before_engine_exit();
bool exit_API_database();

}// end namespace ph
