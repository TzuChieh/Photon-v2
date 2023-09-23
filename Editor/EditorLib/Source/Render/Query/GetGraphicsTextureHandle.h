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

	/*!
	@return The input texture handle. This is part of the query input and can be accessed anytime.
	*/
	TextureHandle getTextureHandle() const;

	ghi::TextureHandle getGraphicsTextureHandle() const;

private:
	Scene* m_scene;
	TextureHandle m_handle;
	ghi::TextureHandle m_gHandle;
};

inline TextureHandle GetGraphicsTextureHandle::getTextureHandle() const
{
	return m_handle;
}

inline ghi::TextureHandle GetGraphicsTextureHandle::getGraphicsTextureHandle() const
{
	return m_gHandle;
}

}// end namespace ph::editor::render
