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
    , m_currentFrameCycleIndex(0)
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

    bool shouldShow = true;
    ImGui::ShowDemoWindow(&shouldShow);

	// TODO: UI

    // Rendering
    ImGui::Render();

    ImDrawData* const drawData = ImGui::GetDrawData();
    PH_ASSERT(drawData);
    m_renderContent->copyNewDrawDataFromMainThread(*drawData, m_currentFrameCycleIndex);

    m_currentFrameCycleIndex = ctx.frameCycleIndex;
}

void ImguiRenderModule::createRenderCommands(RenderThreadCaller& caller)
{
    if(!m_isRenderContentAdded)
    {
        caller.add(
            [renderContent = m_renderContent.get()](RenderData& renderData)
            {
                renderData.scene.addCustomRenderContent(renderContent);
            });

        m_isRenderContentAdded = true;
    }

    

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

    // Setup Dear ImGui context
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

    // Load Fonts
    //io.Fonts->AddFontDefault();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
    ImGui_ImplOpenGL3_Init("version 460");
    ImGui_ImplOpenGL3_Init();

    // DEBUG
    {
        // dummy run to init font

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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Ensure we do not change the original context
    glfwMakeContextCurrent(backupCurrentCtx);
}

}// end namespace ph::editor
