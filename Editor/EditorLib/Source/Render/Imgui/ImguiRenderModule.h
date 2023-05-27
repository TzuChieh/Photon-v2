#pragma once

#include "App/Module/RenderModule.h"
#include "Render/Imgui/ImguiEditorUI.h"
#include "Render/Imgui/ImguiFontLibrary.h"
#include "Render/Imgui/ImguiImageLibrary.h"

#include "ThirdParty/GLFW3_fwd.h"

#include <Math/TVector2.h>

#include <cstddef>
#include <array>
#include <string>

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
	void createSetupRenderCommands(RenderThreadCaller& caller) override;
	void createCleanupRenderCommands(RenderThreadCaller& caller) override;

private:
	void initializeImgui(Editor& editor);
	void initializeImguiFonts(Editor& editor);
	void initializeImguiImages(Editor& editor);
	void terminateImgui();
	void setDisplayFramebufferSizePx(const math::Vector2S& sizePx);

	GLFWwindow* m_glfwWindow;
	ImguiRenderContent* m_renderContent;
	std::string m_configFilePath;
	math::TVector2<uint32> m_displayFramebufferSizePx;
	bool m_isRenderContentAdded;
	std::array<ImWchar, 3> m_fontIconGlyphRanges;

	ImguiEditorUI m_editorUI;
	ImguiFontLibrary m_fontLibrary;
	ImguiImageLibrary m_imageLibrary;
};

inline std::string ImguiRenderModule::getName() const
{
	return "imgui";
}

}// end namespace ph::editor
