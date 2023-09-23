#pragma once

#include "RenderCore/Query/query_basics.h"
#include "RenderCore/ghi_infos.h"

namespace ph::editor::ghi
{

class GetTextureNativeHandle : public QueryPerformer
{
public:
	explicit GetTextureNativeHandle(TextureHandle handle);

	bool performQuery(GraphicsContext& ctx) override;

	TextureNativeHandle getNativeHandle() const;

private:
	TextureHandle m_handle;
	TextureNativeHandle m_nHandle;
};

inline GetTextureNativeHandle::GetTextureNativeHandle(TextureHandle handle)
	: QueryPerformer()
	, m_handle(handle)
	, m_nHandle()
{}

inline TextureNativeHandle GetTextureNativeHandle::getNativeHandle() const
{
	return m_nHandle;
}

}// end namespace ph::editor::ghi
