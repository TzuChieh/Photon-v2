#include "EditorLib/Render/Query/GetGraphicsTextureHandle.h"
#include "EditorLib/Render/System.h"
#include "EditorLib/Render/Scene.h"
#include "EditorLib/Render/Content/Texture.h"

namespace ph::editor::render
{

GetGraphicsTextureHandle::GetGraphicsTextureHandle(TextureHandle handle, Scene* scene)

	: QueryPerformer()

	, m_scene(scene)
	, m_handle(handle)
	, m_gHandle()
{}

bool GetGraphicsTextureHandle::performQuery(System& sys)
{
	Texture* texture = m_scene ? m_scene->getTexture(m_handle) : nullptr;
	if(!texture)
	{
		return false;
	}

	m_gHandle = texture->handle;
	return !m_gHandle.isEmpty();
}

}// end namespace ph::editor::render
