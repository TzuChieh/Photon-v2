#include "DesignerObject.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

DesignerObject::DesignerObject()
	: m_children()
	, m_scene(nullptr)
	, m_name()
	, m_isTicking(false)
{}

DesignerObject::~DesignerObject() = default;

void DesignerObject::onCreate(DesignerScene* const scene)
{
	setScene(scene);
}

void DesignerObject::onRemove()
{
	setScene(nullptr);
}

void DesignerObject::onRenderInit(RenderThreadCaller& caller)
{}

void DesignerObject::onRenderUninit(RenderThreadCaller& caller)
{}

TSpanView<std::shared_ptr<DesignerObject>> DesignerObject::getChildren() const
{
	return m_children;
}

const std::shared_ptr<DesignerObject>& DesignerObject::getChild(const std::size_t childIndex) const
{
	PH_ASSERT_LT(childIndex, m_children.size());
	return m_children[childIndex];
}

std::size_t DesignerObject::numChildren() const
{
	return m_children.size();
}

void DesignerObject::setScene(DesignerScene* const scene)
{
	m_scene = scene;
}

DesignerScene& DesignerObject::getScene()
{
	PH_ASSERT(m_scene != nullptr);
	return *m_scene;
}

const DesignerScene& DesignerObject::getScene() const
{
	PH_ASSERT(m_scene != nullptr);
	return *m_scene;
}

void DesignerObject::setName(std::string name)
{
	m_name = std::move(name);
}

const std::string& DesignerObject::getName() const
{
	return m_name;
}

}// end namespace ph::editor
