#pragma once

#include "RenderCore/Query/query_basics.h"
#include "RenderCore/ghi_infos.h"

namespace ph::editor::ghi
{

class GetTextureNativeHandle : public QueryPerformer
{
public:
	bool performQuery(GraphicsContext& ctx) override;

	GHITextureNativeHandle getNativeHandle() const;

private:
	GHITextureHandle m_textureHandle;
	GHITextureNativeHandle m_nativeHandle;
};

inline GHITextureNativeHandle GetTextureNativeHandle::getNativeHandle() const
{
	return m_nativeHandle;
}

}// end namespace ph::editor::ghi
