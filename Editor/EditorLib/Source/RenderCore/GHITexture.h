#pragma once

#include "RenderCore/ghi_enums.h"
#include "RenderCore/ghi_infos.h"

#include <Math/TVector3.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <variant>

namespace ph::editor
{

class GHIInfoTextureFormat;

class GHITexture
{
public:
	struct MemoryInfo final
	{
		math::Vector3S sizePx = {0, 0, 0};
		std::size_t apparentSize = 0;
		std::size_t deviceSize = 0;
	};

	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	explicit GHITexture(const GHIInfoTextureFormat& format);
	virtual ~GHITexture();

	virtual void upload(
		const std::byte* pixelData, 
		std::size_t numBytes, 
		EGHIPixelComponent componentType) = 0;

	virtual void bind(uint32 slotIndex) = 0;

	virtual MemoryInfo getMemoryInfo() const;
	virtual NativeHandle getNativeHandle();

	const GHIInfoTextureFormat& getFormat() const;

protected:
	void setFormat(const GHIInfoTextureFormat& format);

private:
	GHIInfoTextureFormat m_format;
};

inline auto GHITexture::getMemoryInfo() const
-> MemoryInfo
{
	return MemoryInfo{};
}

inline auto GHITexture::getNativeHandle()
-> NativeHandle
{
	return std::monostate();
}

inline const GHIInfoTextureFormat& GHITexture::getFormat() const
{
	return m_format;
}

}// end namespace ph::editor
