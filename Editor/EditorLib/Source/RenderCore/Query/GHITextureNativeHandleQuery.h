#pragma once

#include "RenderCore/Query/GHIQuery.h"
#include "RenderCore/ghi_infos.h"

namespace ph::editor
{

class GHITextureNativeHandleQuery : public GHIQuery
{
public:
	bool performQuery(GraphicsContext& ctx) override;

	GHITextureNativeHandle getNativeHandle() const;

private:
	GHITextureHandle m_textureHandle;
	GHITextureNativeHandle m_nativeHandle;
};

inline GHITextureNativeHandle GHITextureNativeHandleQuery::getNativeHandle() const
{
	return m_nativeHandle;
}

}// end namespace ph::editor
