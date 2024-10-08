#include "Render/Imgui/ImguiRenderModule.h"
#include "App/Module/ModuleAttachmentInfo.h"
#include "App/Editor.h"
#include "App/Module/MainThreadRenderUpdateContext.h"
#include "Platform/Platform.h"
#include "Platform/PlatformDisplay.h"
#include "Render/Imgui/ImguiRenderContent.h"
#include "Render/RenderThreadCaller.h"
#include "Render/System.h"
#include "Render/Scene.h"
#include "Render/Imgui/imgui_common.h"
//#include "Render/Imgui/Font/IconsMaterialDesign.h"
#include "Render/Imgui/Font/IconsMaterialDesignIcons.h"

#include "ThirdParty/DearImGui.h"
#include "ThirdParty/ImPlot.h"
#include "ThirdParty/GLFW3.h"

#include <ph_core.h>
#include <Common/profiling.h>
#include <Common/os.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/FileSystem/Filesystem.h>

#include <variant>
#include <string_view>

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
	, m_renderContent(nullptr)
	, m_rendererScene(nullptr)
	, m_configFilePath()
	, m_displayFramebufferSizePx(0)
	, m_isRenderContentAdded(false)
	, m_fontIconGlyphRanges()

	, m_editorUI()
	, m_fontLibrary()
	, m_imageLibrary()
{
	m_fontIconGlyphRanges = {
		static_cast<ImWchar>(ICON_MIN_MDI),
		static_cast<ImWchar>(ICON_MAX_MDI),
		static_cast<ImWchar>(0)};
}

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

	if(info.platform->getDisplay().getGraphicsApiType() != ghi::EGraphicsAPI::OpenGL)
	{
		throw_formatted<ModuleException>(
			"no OpenGL support; currently imgui module requires OpenGL");
	}

	// Now we are sure all core components required are there.

	setDisplayFramebufferSizePx(info.framebufferSizePx);

	// Listen to future size change of frame buffer
	info.editor->onDisplayFramebufferResized.addListener(
		[this](const DisplayFramebufferResizedEvent& e)
		{
			setDisplayFramebufferSizePx(e.getNewSizePx());
		});

	// Make sure config directory exist so config file can be saved
	const Path configDirectory = get_imgui_data_directory();
	Filesystem::createDirectories(configDirectory);
	m_configFilePath = (configDirectory / "imgui.ini").toNativeString();

	initializeImgui(*info.editor);
	
	m_editorUI = std::make_unique<ImguiEditorUI>(
		*info.editor, 
		*m_fontLibrary, 
		*m_imageLibrary);
}

void ImguiRenderModule::onDetach()
{
	m_editorUI = nullptr;
	m_imageLibrary = nullptr;
	m_fontLibrary = nullptr;

	terminateImgui();
}

void ImguiRenderModule::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	PH_PROFILE_SCOPE();

	// Start the Dear ImGui frame
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_editorUI->build();

	// DEBUG
	/*m_fontLibrary.useFont(m_fontLibrary.largeFont,
		[]()
		{
			ImGui::Button(ICON_MD_FOLDER_COPY " Search");
		});

	ImGui::Button(ICON_MD_FOLDER_COPY " Search");
	ImGui::Button(ICON_MD_GRADE " Search");
	ImGui::Button(ICON_MD_HOTEL " Search");*/

	// Rendering
	ImGui::Render();

	ImDrawData* const mainThreadDrawData = ImGui::GetDrawData();
	PH_ASSERT(mainThreadDrawData);

	// We never want to block main thread. If so, consider increase the size of shared data.
	PH_ASSERT(m_renderContent);
	PH_ASSERT(m_renderContent->getSharedRenderData().mayWaitToProduce() == false);

	// Copy draw data to a buffer shared with GHI thread
	m_renderContent->getSharedRenderData().guardedProduce(
		[mainThreadDrawData](render::ImguiRenderContent::ImguiRenderData& renderData)
		{
			renderData.copyFrom(*mainThreadDrawData);
		});
}

void ImguiRenderModule::createSetupRenderCommands(RenderThreadCaller& caller)
{
	PH_PROFILE_SCOPE();

	// Create the renderer scene if not already present. Must be the first thing to do, so subsequent
	// operations can access the renderer scene.
	if(!m_rendererScene)
	{
		auto rendererScene = std::make_unique<render::Scene>("ImGui Scene");
		m_rendererScene = rendererScene.get();

		caller.add(
			[scene = std::move(rendererScene)](render::System& sys) mutable
			{
				sys.addScene(std::move(scene));
			});
	}

	// Create and add the IMGUI render content to render thread
	if(!m_renderContent)
	{
		auto renderContent = std::make_unique<render::ImguiRenderContent>();
		m_renderContent = renderContent.get();

		caller.add(
			[renderContent = std::move(renderContent), scene = m_rendererScene](render::System& sys) mutable
			{
				scene->addDynamicResource(std::move(renderContent));
			});
	}
}

void ImguiRenderModule::createRenderCommands(RenderThreadCaller& caller)
{
	PH_PROFILE_SCOPE();

	PH_ASSERT(m_rendererScene);
	m_imageLibrary->createRenderCommands(caller, *m_rendererScene);

	// Need to notify render thread that there is new render data for GHI
	caller.add(
		[renderContent = m_renderContent](render::System& sys)
		{
			PH_ASSERT(renderContent);
			renderContent->signifyNewRenderDataIsAvailable();
		});
}

void ImguiRenderModule::createCleanupRenderCommands(RenderThreadCaller& caller)
{
	PH_PROFILE_SCOPE();

	if(m_renderContent)
	{
		caller.add(
			[renderContent = m_renderContent, scene = m_rendererScene](render::System& sys) mutable
			{
				PH_ASSERT(scene);
				scene->removeResource(renderContent);
			});
	}

	if(m_rendererScene)
	{
		m_imageLibrary->cleanupTextures(caller, *m_rendererScene);
	}

	// Remove the renderer scene in the end, after all other operations are done with it.
	if(m_rendererScene)
	{
		caller.add(
			[scene = m_rendererScene](render::System& sys)
			{
				sys.removeScene(scene);
			});
		m_rendererScene = nullptr;
	}
}

void ImguiRenderModule::setDisplayFramebufferSizePx(const math::Vector2S& sizePx)
{
	m_displayFramebufferSizePx = sizePx.losslessCast<uint32>();
}

void ImguiRenderModule::initializeImgui(Editor& editor)
{
    // Ensure we do not change the original context
    PH_ASSERT(m_glfwWindow);
    GLFWwindow* const backupCurrentCtx = glfwGetCurrentContext();
    glfwMakeContextCurrent(m_glfwWindow);

	// DEBUG
	//glfwSwapInterval(1); // Enable vsync

    PH_LOG(DearImGui, Note, "setting-up context...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;// Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
	io.IniFilename = m_configFilePath.c_str();

    // We do not support viewports, its frame update functions require changing the current 
    // OpenGL context
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
   
	initializeImguiFonts(editor);
	initializeImguiImages(editor);

	PH_LOG(DearImGui, Note, "setting-up style...");

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	PH_LOG(DearImGui, Note, "setting-up platform renderer backends...");

	auto shaderVersionHeader = "#version 460";
	if constexpr(PH_OPERATING_SYSTEM_IS_LINUX)
	{
		shaderVersionHeader = "#version 450";
	}

	ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
	ImGui_ImplOpenGL3_Init(shaderVersionHeader);

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
	PH_LOG(DearImGui, Note, "setting-up fonts...");

	constexpr std::string_view BASE_FONT_FILENAME = "Arimo[wght].ttf";
	//constexpr std::string_view FONT_ICON_FILENAME = FONT_ICON_FILE_NAME_MDI;
	constexpr std::string_view FONT_ICON_FILENAME = "materialdesignicons-webfont.ttf";

	m_fontLibrary = std::make_unique<ImguiFontLibrary>();

	ImGuiIO& io = ImGui::GetIO();

	const Path fontDirectory = get_internal_resource_directory(EEngineProject::EditorLib) / "Font";
	const float fontSizePx = editor.dimensionHints.fontSize;
	const float iconFontSizePx = fontSizePx * 1.2f;
	const float largeFontRatio = editor.dimensionHints.largeFontSize / editor.dimensionHints.fontSize;

	// Loading default font
	//io.Fonts->AddFontDefault();
	m_fontLibrary->defaultFont = io.Fonts->AddFontFromFileTTF(
		(fontDirectory / BASE_FONT_FILENAME).toString().c_str(),
		fontSizePx);
	io.FontDefault = m_fontLibrary->defaultFont;

	// Loading icon font--merge with default font
	ImFontConfig iconFontConfig;
	iconFontConfig.MergeMode = true;
	iconFontConfig.PixelSnapH = true;
	iconFontConfig.GlyphMinAdvanceX = iconFontSizePx;
	iconFontConfig.GlyphOffset.x = 0.0f;
	iconFontConfig.GlyphOffset.y = iconFontSizePx * 0.075f;
	io.Fonts->AddFontFromFileTTF(
		(fontDirectory / FONT_ICON_FILENAME).toString().c_str(),
		iconFontSizePx,
		&iconFontConfig,
		m_fontIconGlyphRanges.data());

	// Loading large default font
	m_fontLibrary->largeFont = io.Fonts->AddFontFromFileTTF(
		(fontDirectory / BASE_FONT_FILENAME).toString().c_str(),
		fontSizePx * largeFontRatio);

	// Loading large font icon--merge with large default font
	ImFontConfig largeIconFontConfig;
	largeIconFontConfig.MergeMode = true;
	largeIconFontConfig.PixelSnapH = true;
	largeIconFontConfig.GlyphMinAdvanceX = iconFontConfig.GlyphMinAdvanceX * largeFontRatio;
	largeIconFontConfig.GlyphOffset.x = iconFontConfig.GlyphOffset.x * largeFontRatio;
	largeIconFontConfig.GlyphOffset.y = iconFontConfig.GlyphOffset.y * largeFontRatio;
	io.Fonts->AddFontFromFileTTF(
		(fontDirectory / FONT_ICON_FILENAME).toString().c_str(),
		iconFontSizePx * largeFontRatio,
		&largeIconFontConfig,
		m_fontIconGlyphRanges.data());

	if(!m_fontLibrary->defaultFont || !m_fontLibrary->largeFont)
	{
		PH_LOG(DearImGui, Error,
			"font initialization failed (default font valid: {}, large font valid: {})",
			m_fontLibrary->defaultFont != nullptr, m_fontLibrary->largeFont != nullptr);
	}
}

void ImguiRenderModule::initializeImguiImages(Editor& editor)
{
	PH_LOG(DearImGui, Note, "setting-up images...");

	m_imageLibrary = std::make_unique<ImguiImageLibrary>();
	Path imageDirectory = get_internal_resource_directory(EEngineProject::EditorLib) / "Image";

	m_imageLibrary->loadImage(EImguiImage::Warning, imageDirectory / "hazard-sign.png");
	m_imageLibrary->loadImage(EImguiImage::Folder, imageDirectory / "open-folder.png");
	m_imageLibrary->loadImage(EImguiImage::File, imageDirectory / "database.png");
	m_imageLibrary->loadImage(EImguiImage::Image, imageDirectory / "mona-lisa.png");
}

void ImguiRenderModule::terminateImgui()
{
    PH_ASSERT(m_glfwWindow);

    // Ensure we do not change the original context
    GLFWwindow* const backupCurrentCtx = glfwGetCurrentContext();
    glfwMakeContextCurrent(m_glfwWindow);

    PH_LOG(DearImGui, Note, "cleaning up...");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // Ensure we do not change the original context
    glfwMakeContextCurrent(backupCurrentCtx);
}

}// end namespace ph::editor
