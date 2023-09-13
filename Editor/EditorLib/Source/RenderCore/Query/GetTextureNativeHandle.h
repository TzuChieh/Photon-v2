#pragma once

#include "RenderCore/Query/query_basics.h"
#include "RenderCore/ghi_infos.h"

namespace ph::editor::ghi
{

class GetTextureNativeHandle : public QueryPerformer
{
public:
	explicit GetTextureNativeHandle(GHITextureHandle handle);

	bool performQuery(GraphicsContext& ctx) override;

	GHITextureNativeHandle getNativeHandle() const;

private:
	GHITextureHandle m_handle;
	GHITextureNativeHandle m_nHandle;
};

inline GetTextureNativeHandle::GetTextureNativeHandle(GHITextureHandle handle)
	: QueryPerformer()
	, m_handle(handle)
	, m_nHandle()
{}

inline GHITextureNativeHandle GetTextureNativeHandle::getNativeHandle() const
{
	return m_nHandle;
}

}// end namespace ph::editor::ghi
