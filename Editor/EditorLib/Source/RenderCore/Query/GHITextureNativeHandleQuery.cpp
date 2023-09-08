#include "RenderCore/Query/GHITextureNativeHandleQuery.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"

namespace ph::editor
{

bool GHITextureNativeHandleQuery::performQuery(GraphicsContext& ctx)
{
	m_nativeHandle = ctx.getGHI().tryGetTextureNativeHandle(m_textureHandle);
	return !std::holds_alternative<std::monostate>(m_nativeHandle);
}

}// end namespace ph::editor
