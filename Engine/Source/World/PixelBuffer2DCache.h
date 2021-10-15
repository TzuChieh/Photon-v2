#pragma once

#include "Core/Texture/Pixel/PixelBuffer2D.h"
#include "Core/Texture/Pixel/pixel_texture_basics.h"
#include "Utility/INoCopyAndMove.h"

#include <memory>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <atomic>

namespace ph
{

struct PixelBuffer2DCacheEntry final : private INoCopyAndMove
{
	std::shared_ptr<PixelBuffer2D> buffer;
	pixel_texture::EPixelLayout    defaultLayout;
};

class PixelBuffer2DCache final
{
public:
	PixelBuffer2DCache();

	void addBuffer(std::string name, PixelBuffer2DCacheEntry bufferEntry);
	std::size_t estimateBufferMemoryUsageBytes() const;

private:
	std::atomic<std::size_t> m_bufferMemoryBytesCounter;
};

}// end namespace ph
