#pragma once

#include "RenderCore/ghi_enums.h"
#include "RenderCore/ghi_states.h"

#include <Math/TVector2.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <variant>

namespace ph::editor
{

class GHIInfoTextureFormat final
{
public:
	EGHIInfoPixelFormat pixelFormat;
	GHIInfoSampleState sampleState;

	GHIInfoTextureFormat();
};

class GHITexture2D
{
public:
	struct MemoryInfo final
	{
		math::Vector2S sizePx = {0, 0};
		std::size_t apparentSize = 0;
		std::size_t deviceSize = 0;
	};

	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	GHITexture2D();
	virtual ~GHITexture2D();

	virtual void upload(
		const std::byte* pixelData, 
		std::size_t numBytes, 
		EGHIInfoPixelComponent componentType) = 0;

	virtual void bind(uint32 slotIndex) = 0;

	virtual MemoryInfo getMemoryInfo() const;
	virtual NativeHandle getNativeHandle();
};

inline auto GHITexture2D::getMemoryInfo() const
-> MemoryInfo
{
	return MemoryInfo{};
}

}// end namespace ph::editor
