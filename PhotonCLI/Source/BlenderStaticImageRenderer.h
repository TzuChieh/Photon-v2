#pragma once

#include "StaticImageRenderer.h"

#include <Common/primitive_type.h>

#include <thread>
#include <atomic>
#include <stop_token>

namespace ph::cli
{

/*! @brief Renderer for working with Blender.
*/
class BlenderStaticImageRenderer : public StaticImageRenderer
{
public:
	explicit BlenderStaticImageRenderer(const ProcessedArguments& args);

	void render() override;

private:
	std::jthread makeStatsThread();
	std::jthread makeServerThread(uint16 port, float32 peekIntervalS);
	void runServer(std::stop_token token, uint16 port, float32 peekIntervalS);

	PhUInt32 m_imageWidthPx;
	PhUInt32 m_imageHeightPx;
	std::atomic_flag m_serverStartPeekingFlag;
};

}// end namespace ph::cli
