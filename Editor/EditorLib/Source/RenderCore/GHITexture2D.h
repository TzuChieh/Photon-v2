#pragma once

#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

#include <cstddef>

namespace ph::editor
{

class GHITexture2D
{
public:
	GHITexture2D();
	GHITexture2D(math::TVector2<uint32> sizePx, EGHITextureFormat format);
	virtual ~GHITexture2D();

	virtual void loadPixels(void* pixelData, std::size_t numBytes) = 0;
	virtual std::size_t sizeInBytes() const = 0;

	virtual void* getNativeHandle();

	math::TVector2<uint32> getSizePx() const;
	EGHITextureFormat getFormat() const;

private:
	math::TVector2<uint32> m_sizePx;
	EGHITextureFormat      m_format;
};

inline void* GHITexture2D::getNativeHandle()
{
	return nullptr;
}

inline math::TVector2<uint32> GHITexture2D::getSizePx() const
{
	return m_sizePx;
}

inline EGHITextureFormat GHITexture2D::getFormat() const
{
	return m_format;
}

}// end namespace ph::editor
