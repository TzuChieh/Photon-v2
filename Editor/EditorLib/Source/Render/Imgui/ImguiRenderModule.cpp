#include "Render/Imgui/ImguiRenderModule.h"
#include "App/Module/ModuleAttachmentInfo.h"
#include "App/Editor.h"
#include "App/Module/MainThreadRenderUpdateContext.h"
#include "ThirdParty/DearImGui.h"
#include "ThirdParty/GLFW3.h"
#include "Platform/Platform.h"
#include "Platform/PlatformDisplay.h"
#include "Render/Imgui/ImguiRenderContent.h"
#include "EditorCore/Thread/RenderThreadCaller.h"
#include "RenderCore/RenderData.h"
#include "RenderCore/RTRScene.h"
#include "Render/Imgui/imgui_common.h"

#include <ph_cpp_core.h>

#include <variant>

/*
Note that the implementation here is quite sensitive to the windowing system and corresponding graphics
context. The comment for function `ImGui::Render()` states that anything under the `ImGui` namespce 
does not use the graphics API (GHI in our term) in anyway. This precondition must be kept in mind.
*/

namespace ph::editor
{

ImguiRenderModule::ImguiRenderModule()
    : RenderModule()
    , m_glfwWindow(nullptr)
    , m_frameBufferSizePx(0)
    , m_renderContent(nullptr)
    , m_isRenderContentAdded(false)
    , m_editorUI()
{}

ImguiRenderModule::~ImguiRenderModule() = default;

void ImguiRenderModule::onAttach(const ModuleAttachmentInfo& info)
{
    auto nativeWindow = info.platform->getDisplay().getNativeWindow();
    if(!std::holds_alternative<GLFWwindow*>(nativeWindow))
    {
        throw_formatted<ModuleException>(
            "no GLFW window; currently imgui module requires GLFW");
    }

    m_glfwWindow = std::get<GLFWwindow*>(nativeWindow);
    PH_ASSERT(m_glfwWindow);

    if(info.platform->getDisplay().getGraphicsAPIType() != EGraphicsAPI::OpenGL)
    {
        throw_formatted<ModuleException>(
            "no OpenGL support; currently imgui module requires OpenGL");
    }

    // Now we are sure all core components required are there.

	setFrameBufferSizePx(info.frameBufferSizePx);

	// Listen to future size change of frame buffer
	info.editor->onFrameBufferResize.addListener(
		[this](const FrameBufferResizeEvent& e)
		{
			setFrameBufferSizePx(e.getNewSizePx());
		});

	initializeImgui();

    m_renderContent = std::make_unique<ImguiRenderContent>();
    m_editorUI.setEditor(info.editor);
}

void ImguiRenderModule::onDetach()
{
    terminateImgui();
}

void ImguiRenderModule::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
    // Start the Dear ImGui frame
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    m_editorUI.build();

    // Rendering
    ImGui::Render();

    ImDrawData* const mainThreadDrawData = ImGui::GetDrawData();
    PH_ASSERT(mainThreadDrawData);

    // We never want to block main thread. If so, consider increase the size of shared data.
    PH_ASSERT(m_renderContent->getSharedRenderData().mayWaitToProduce() == false);

    // Copy draw data to a buffer shared with GHI thread
    m_renderContent->getSharedRenderData().guardedProduce(
        [mainThreadDrawData](ImguiRenderContent::ImguiRenderData& renderData)
        {
            renderData.copyFrom(*mainThreadDrawData);
        });
}

void ImguiRenderModule::createRenderCommands(RenderThreadCaller& caller)
{
    // Add the IMGUI render content to render thread if not already present
    if(!m_isRenderContentAdded)
    {
        caller.add(
            [renderContent = m_renderContent.get()](RenderData& renderData)
            {
                renderData.scene.addCustomRenderContent(renderContent);
            });

        m_isRenderContentAdded = true;
    }

    // Need to notify render thread that there is new render data for GHI
    caller.add(
        [this](RenderData& renderData)
        {
            m_renderContent->signifyNewRenderDataIsAvailable();
        });

    // TODO: remove content before detach
}

void ImguiRenderModule::setFrameBufferSizePx(const math::Vector2S& sizePx)
{
	m_frameBufferSizePx = sizePx.safeCast<uint32>();
}

void ImguiRenderModule::initializeImgui()
{
    PH_ASSERT(m_glfwWindow);
    
    // Ensure we do not change the original context
    GLFWwindow* const backupCurrentCtx = glfwGetCurrentContext();
    glfwMakeContextCurrent(m_glfwWindow);

    PH_LOG(DearImGui, "setting-up context...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;// Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

    // We do not support viewports, its frame update functions require changing the current 
    // OpenGL context
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    PH_LOG(DearImGui, "setting-up fonts...");

    //io.Fonts->AddFontDefault();
    io.FontDefault = io.Fonts->AddFontFromFileTTF(
        (get_internal_resource_directory(EEngineProject::EditorLib) / "Font" / "Arial-Regular.ttf").toString().c_str(),
        15.0f);

    PH_LOG(DearImGui, "setting-up style...");

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    PH_LOG(DearImGui, "setting-up platform renderer backends...");

    ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // A single-frame dummy run to initialize some internal structures
    // (although we can manually call backend functions such as `ImGui_ImplOpenGL3_CreateFontsTexture()` 
    // and `ImGui_ImplOpenGL3_CreateDeviceObjects()` etc., a dummy run that do nothing is more
    // convenient and somewhat more robust to future library updates)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // do nothing

        ImGui::Render();
    }

    // Ensure we do not change the original context
    glfwMakeContextCurrent(backupCurrentCtx);
}

void ImguiRenderModule::terminateImgui()
{
    PH_ASSERT(m_glfwWindow);

    // Ensure we do not change the original context
    GLFWwindow* const backupCurrentCtx = glfwGetCurrentContext();
    glfwMakeContextCurrent(m_glfwWindow);

    PH_LOG(DearImGui, "cleaning up...");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Ensure we do not change the original context
    glfwMakeContextCurrent(backupCurrentCtx);
}

}// end namespace ph::editor
