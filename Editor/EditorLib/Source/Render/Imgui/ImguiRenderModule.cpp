#include "Render/Imgui/ImguiRenderModule.h"
#include "App/Module/ModuleAttachmentInfo.h"
#include "App/Editor.h"
#include "App/Module/MainThreadRenderUpdateContext.h"
#include "ThirdParty/DearImGui.h"
#include "ThirdParty/GLFW3.h"
#include "Platform/Platform.h"
#include "Platform/PlatformDisplay.h"
#include "Render/Imgui/ImguiRenderContent.h"
#include "Render/RenderThreadCaller.h"
#include "Render/RenderData.h"
#include "Render/RTRScene.h"
#include "Render/Imgui/imgui_common.h"
#include "Render/Imgui/Font/IconsMaterialDesign.h"

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
	, m_framebufferSizePx(0)
	, m_renderContent(nullptr)
	, m_isRenderContentAdded(false)
	, m_editorUI()
	, m_helper()
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

	if(info.platform->getDisplay().getGraphicsApiType() != EGraphicsAPI::OpenGL)
	{
		throw_formatted<ModuleException>(
			"no OpenGL support; currently imgui module requires OpenGL");
	}

	// Now we are sure all core components required are there.

	setFramebufferSizePx(info.framebufferSizePx);

	// Listen to future size change of frame buffer
	info.editor->onFramebufferResize.addListener(
		[this](const FramebufferResizeEvent& e)
		{
			setFramebufferSizePx(e.getNewSizePx());
		});

	initializeImgui(*info.editor);
	m_editorUI.initialize(info.editor, &m_helper);
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

	m_helper.useFont(m_helper.largeFont, 
		[]()
		{
			ImGui::Button(ICON_MD_FOLDER_COPY " Search");
		});

	ImGui::Button(ICON_MD_FOLDER_COPY " Search");
	ImGui::Button(ICON_MD_GRADE " Search");
	ImGui::Button(ICON_MD_HOTEL " Search");

	// Rendering
	ImGui::Render();

	ImDrawData* const mainThreadDrawData = ImGui::GetDrawData();
	PH_ASSERT(mainThreadDrawData);

	if(m_renderContent)
	{
		// We never want to block main thread. If so, consider increase the size of shared data.
		PH_ASSERT(m_renderContent->getSharedRenderData().mayWaitToProduce() == false);

		// Copy draw data to a buffer shared with GHI thread
		m_renderContent->getSharedRenderData().guardedProduce(
			[mainThreadDrawData](ImguiRenderContent::ImguiRenderData& renderData)
			{
				renderData.copyFrom(*mainThreadDrawData);
			});
	}
}

void ImguiRenderModule::createRenderCommands(RenderThreadCaller& caller)
{
	// Create and add the IMGUI render content to render thread if not already present
	if(!m_renderContent)
	{
		auto renderContent = std::make_unique<ImguiRenderContent>();
		m_renderContent = renderContent.get();

		caller.add(
			[renderContent = std::move(renderContent)](RenderData& renderData) mutable
			{
				renderData.scene.addCustomRenderContent(std::move(renderContent));
			});

		// Early out since we just added the render content, a render update has not been performed 
		// so no commands to add
		return;
	}

	// Need to notify render thread that there is new render data for GHI
	caller.add(
		[this](RenderData& renderData)
		{
			m_renderContent->signifyNewRenderDataIsAvailable();
		});

	// TODO: remove content before detach
}

void ImguiRenderModule::setFramebufferSizePx(const math::Vector2S& sizePx)
{
	m_framebufferSizePx = sizePx.safeCast<uint32>();
}

void ImguiRenderModule::initializeImgui(Editor& editor)
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
   
	initializeImguiFonts(editor);

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

void ImguiRenderModule::initializeImguiFonts(Editor& editor)
{
	PH_LOG(DearImGui, "setting-up fonts...");

	ImGuiIO& io = ImGui::GetIO();

	const Path fontDirectory = get_internal_resource_directory(EEngineProject::EditorLib) / "Font";
	const float fontSizePx = editor.dimensionHints.fontSize;
	const float largeFontRatio = editor.dimensionHints.largeFontSize / editor.dimensionHints.fontSize;

	// Loading default font
	//io.Fonts->AddFontDefault();
	m_helper.defaultFont = io.Fonts->AddFontFromFileTTF(
		(fontDirectory / "Arial-Regular.ttf").toString().c_str(),
		fontSizePx);
	io.FontDefault = m_helper.defaultFont;

	// Loading icon font--merge with default font
	ImFontConfig iconFontConfig;
	iconFontConfig.MergeMode = true;
	iconFontConfig.PixelSnapH = true;
	iconFontConfig.GlyphMinAdvanceX = fontSizePx;
	iconFontConfig.GlyphOffset.x = 0.0f;
	iconFontConfig.GlyphOffset.y = 2.2f * (fontSizePx / 15.0f);
	static const ImWchar iconFontRanges[] = {
		static_cast<ImWchar>(ICON_MIN_MD), 
		static_cast<ImWchar>(ICON_MAX_MD), 
		static_cast<ImWchar>(0)};
	io.Fonts->AddFontFromFileTTF(
		(fontDirectory / FONT_ICON_FILE_NAME_MD).toString().c_str(),
		fontSizePx,
		&iconFontConfig,
		iconFontRanges);

	// Loading large default font
	m_helper.largeFont = io.Fonts->AddFontFromFileTTF(
		(fontDirectory / "Arial-Regular.ttf").toString().c_str(),
		fontSizePx * largeFontRatio);

	// Loading large font icon--merge with large default font
	ImFontConfig largeIconFontConfig;
	largeIconFontConfig.MergeMode = true;
	largeIconFontConfig.PixelSnapH = true;
	largeIconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMinAdvanceX * largeFontRatio;
	largeIconFontConfig.GlyphOffset.x = iconFontConfig.GlyphOffset.x * largeFontRatio;
	largeIconFontConfig.GlyphOffset.y = iconFontConfig.GlyphOffset.y * largeFontRatio;
	io.Fonts->AddFontFromFileTTF(
		(fontDirectory / FONT_ICON_FILE_NAME_MD).toString().c_str(),
		fontSizePx * largeFontRatio,
		&largeIconFontConfig,
		iconFontRanges);
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
