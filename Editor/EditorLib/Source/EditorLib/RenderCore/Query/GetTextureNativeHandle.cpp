#include "EditorLib/RenderCore/Query/GetTextureNativeHandle.h"
#include "EditorLib/RenderCore/GraphicsContext.h"
#include "EditorLib/RenderCore/GHI.h"

namespace ph::editor::ghi
{

bool GetTextureNativeHandle::performQuery(GraphicsContext& ctx)
{
	m_nHandle = ctx.getGHI().tryGetTextureNativeHandle(m_handle);
	return !std::holds_alternative<std::monostate>(m_nHandle);
}

}// end namespace ph::editor::ghi
