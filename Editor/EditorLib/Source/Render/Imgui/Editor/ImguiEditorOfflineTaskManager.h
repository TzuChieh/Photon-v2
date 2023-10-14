#pragma once

#include "Render/Renderer/OfflineRenderStats.h"
#include "Render/Renderer/OfflineRenderPeek.h"
#include "Render/Imgui/ImguiEditorPanel.h"

#include <cstddef>
#include <vector>
#include <string>

namespace ph::editor::render { class OfflineRenderer; }

namespace ph::editor
{

class ImguiEditorUIProxy;
class DesignerScene;
class RenderAgent;

class ImguiEditorOfflineTaskManager : public ImguiEditorPanel
{
public:
	explicit ImguiEditorOfflineTaskManager(ImguiEditorUIProxy editorUI);

	void buildWindow(const char* windowIdName, bool* isOpening) override;
	auto getAttributes() const -> Attributes override;

private:
	void buildTaskInfoContent(DesignerScene* scene);
	void buildTaskDetailContent();

	struct TaskInfo
	{
		RenderAgent* agent = nullptr;
		render::OfflineRenderer* renderer = nullptr;
		std::string outputImageName;

		/*! @brief Dynamically obtained states.
		*/
		///@{
		render::EOfflineRenderStage stage = render::EOfflineRenderStage::Standby;
		render::OfflineRenderStats stats;
		render::OfflineRenderPeek peek;
		///@}
	};

	std::vector<TaskInfo> m_taskInfos;
	std::size_t m_numValidTaskInfos;
	std::size_t m_selectedTaskInfoIdx;
	bool m_enableStopRender;
	bool m_enableRenderPreview;
	bool m_indicateUpdatedRegions;
	bool m_autoSyncImageView;
};

}// end namespace ph::editor
