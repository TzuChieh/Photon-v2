#pragma once

#include "Render/Query/query_basics.h"
#include "Render/Content/fwd.h"
#include "RenderCore/ghi_infos.h"

#include <cstddef>

namespace ph::editor::render
{

class Scene;

class GetGraphicsTextureHandle : public QueryPerformer
{
public:
	GetGraphicsTextureHandle(TextureHandle handle, Scene* scene);

	bool performQuery(System& sys) override;

	GHITextureHandle getGraphicsTextureHandle() const;

private:
	Scene* m_scene;
	TextureHandle m_handle;
	GHITextureHandle m_gHandle;
};

inline GHITextureHandle GetGraphicsTextureHandle::getGraphicsTextureHandle() const
{
	return m_gHandle;
}

}// end namespace ph::editor::render
