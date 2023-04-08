#include "Render/RenderData.h"

#include <memory>

namespace ph::editor
{

RenderData::RenderData()
	: updateCtx()
	, scenes()
	, m_persistentScene(nullptr)
{
	m_persistentScene = scenes.add(std::make_unique<RendererScene>());
}

}// end namespace ph::editor
