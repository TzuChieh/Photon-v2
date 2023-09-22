#pragma once

#include "Render/Renderer/OfflineRenderStats.h"

#include <cstddef>
#include <vector>
#include <string>

namespace ph::editor::render { class OfflineRenderer; }

namespace ph::editor
{

class ImguiEditorUIProxy;
class DesignerScene;
class DesignerObject;

class ImguiEditorOfflineTaskManager final
{
public:
	ImguiEditorOfflineTaskManager();

	void buildWindow(
		const char* title, 
		ImguiEditorUIProxy editorUI,
		bool* isOpening = nullptr);

private:
	void buildTaskInfoContent(DesignerScene* scene);
	void buildTaskDetailContent();

	struct TaskInfo
	{
		DesignerObject* ownerObj = nullptr;
		render::OfflineRenderer* renderer = nullptr;
		render::EOfflineRenderStage stage = render::EOfflineRenderStage::Standby;
		render::OfflineRenderStats stats;
	};

	std::vector<TaskInfo> m_taskInfos;
	std::size_t m_numValidTaskInfos;
	std::size_t m_selectedTaskInfoIdx;
};

}// end namespace ph::editor
