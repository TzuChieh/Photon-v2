#pragma once

#include "App/Module/RenderModule.h"
#include "ThirdParty/GLFW3_fwd.h"
#include "Render/Imgui/ImguiEditorUI.h"
#include "Render/Imgui/ImguiHelper.h"

#include <Math/TVector2.h>

#include <memory>
#include <cstddef>

namespace ph::editor
{

class Editor;
class ImguiRenderContent;

class ImguiRenderModule : public RenderModule
{
public:
	ImguiRenderModule();
	~ImguiRenderModule() override;

	std::string getName() const override;
	void onAttach(const ModuleAttachmentInfo& info) override;
	void onDetach() override;
	void renderUpdate(const MainThreadRenderUpdateContext& ctx) override;
	void createRenderCommands(RenderThreadCaller& caller) override;

private:
	void initializeImgui(Editor& editor);
	void initializeImguiFonts(Editor& editor);
	void terminateImgui();
	void setFramebufferSizePx(const math::Vector2S& sizePx);

	GLFWwindow*                         m_glfwWindow;
	math::TVector2<uint32>              m_framebufferSizePx;
	std::unique_ptr<ImguiRenderContent> m_renderContent;
	bool                                m_isRenderContentAdded;
	ImguiEditorUI                       m_editorUI;
	ImguiHelper                         m_helper;
};

inline std::string ImguiRenderModule::getName() const
{
	return "imgui";
}

}// end namespace ph::editor
