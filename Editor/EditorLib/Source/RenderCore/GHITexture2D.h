#pragma once

#include "RenderCore/ghi_enums.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>

#include <cstddef>

namespace ph::editor
{

class GHITexture2D
{
public:
	struct MemoryInfo final
	{
		math::Vector2S sizePx = {0, 0};
		std::size_t apparentSize = 0;
		std::size_t deviceSize = 0;
	};

	GHITexture2D();
	virtual ~GHITexture2D();

	virtual void upload(void* pixelData, std::size_t numBytes) = 0;
	virtual void bind(uint32 slotIndex) = 0;

	virtual MemoryInfo getMemoryInfo() const;
	virtual void* getNativeHandle();
};

inline void* GHITexture2D::getNativeHandle()
{
	return nullptr;
}

inline auto GHITexture2D::getMemoryInfo() const
-> MemoryInfo
{
	return MemoryInfo{};
}

}// end namespace ph::editor
