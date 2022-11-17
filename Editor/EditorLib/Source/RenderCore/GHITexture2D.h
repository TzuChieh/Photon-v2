#pragma once

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

namespace ph::editor
{

class GHITexture2D
{
public:
	GHITexture2D();
	explicit GHITexture2D(math::TVector2<uint32> sizePx);
	virtual ~GHITexture2D();

	virtual void* getNativeHandle();

	math::TVector2<uint32> getSizePx() const;

private:
	math::TVector2<uint32> m_sizePx;
};

inline void* GHITexture2D::getNativeHandle()
{
	return nullptr;
}

inline math::TVector2<uint32> GHITexture2D::getSizePx() const
{
	return m_sizePx;
}

}// end namespace ph::editor
