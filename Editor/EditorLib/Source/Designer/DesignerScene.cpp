#include "Designer/DesignerScene.h"

namespace ph::editor
{

DesignerScene::DesignerScene()
	: m_editor(nullptr)
{}

DesignerScene::~DesignerScene() = default;

void DesignerScene::setEditor(Editor* const editor)
{
	m_editor = editor;
}

}// end namespace ph::editor
