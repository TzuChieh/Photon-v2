#include "RenderCore/Query/GetTextureNativeHandle.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"

namespace ph::editor::ghi
{

bool GetTextureNativeHandle::performQuery(GraphicsContext& ctx)
{
	m_nativeHandle = ctx.getGHI().tryGetTextureNativeHandle(m_textureHandle);
	return !std::holds_alternative<std::monostate>(m_nativeHandle);
}

}// end namespace ph::editor::ghi
