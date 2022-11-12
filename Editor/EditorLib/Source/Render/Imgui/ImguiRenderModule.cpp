#include "Render/Imgui/ImguiRenderModule.h"
#include "App/Module/ModuleAttachmentInfo.h"
#include "App/Editor.h"
#include "App/Module/MainThreadRenderUpdateContext.h"

#include <ThirdParty/DearImGui.h>

namespace ph::editor
{

void ImguiRenderModule::onAttach(const ModuleAttachmentInfo& info)
{
	setFrameBufferSizePx(info.frameBufferSizePx);

	// Listen to future size change of frame buffer
	info.editor->onFrameBufferResize.addListener(
		[this](const FrameBufferResizeEvent& e)
		{
			setFrameBufferSizePx(e.getNewSizePx());
		});

	initializeImgui();
}

void ImguiRenderModule::onDetach()
{
	// TODO
}

void ImguiRenderModule::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	// TODO
}

void ImguiRenderModule::setFrameBufferSizePx(const math::Vector2S& sizePx)
{
	m_frameBufferSizePx = sizePx.safeCast<uint32>();
}

void ImguiRenderModule::initializeImgui()
{
    //// Setup Dear ImGui context
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    ////io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    ////io.ConfigViewportsNoAutoMerge = true;
    ////io.ConfigViewportsNoTaskBarIcon = true;

    //// Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ////ImGui::StyleColorsLight();

    //// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    //ImGuiStyle& style = ImGui::GetStyle();
    //if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //{
    //    style.WindowRounding = 0.0f;
    //    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    //}
}

void ImguiRenderModule::terminateImgui()
{
    //// Cleanup
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    //ImGui::DestroyContext();
}

}// end namespace ph::editor
