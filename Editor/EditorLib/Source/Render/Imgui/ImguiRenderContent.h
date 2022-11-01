#pragma once

#include "RenderCore/CustomRenderContent.h"
#include "editor_lib_config.h"

#include <imgui.h>

#include <array>
#include <cstddef>
#include <vector>

namespace ph::editor
{

class ImguiRenderContent : public CustomRenderContent
{
public:
	ImguiRenderContent();

	void update(const RenderThreadUpdateContext& ctx) override;
	void createGHICommands(GHIThreadCaller& caller) override;

	void copyNewDrawDataFromMainThread(const ImDrawData& srcDrawData, std::size_t frameCycleIndex);

private:
	// All data required by IMGUI to render a frame.
	struct ImguiRenderData
	{
		ImDrawData               drawData;
		std::vector<ImDrawList>  drawListBuffer;
		std::vector<ImDrawList*> drawListPtrBuffer;
		ImGuiViewport            ownerViewportBuffer;

		void copyFrom(const ImDrawData& srcDrawData);
	};

	/*!
	The array of IMGUI render data are the core of N-buffered rendering, where N is the number of 
	buffered frames. This enables rendering IMGUI from a separate thread (in our case the render
	thread) without conflicting concurrent access to the `ImDrawData` on main thread (the one 
	obtained from `ImGui::GetDrawData()`).

	References: 
	[1] Add a helper to simplify and optimize backing up the render frame
	    https://github.com/ocornut/imgui/pull/2433
	[2] https://twitter.com/ocornut/status/973093439666520064
	[3] https://twitter.com/ocornut/status/975096904446021633
	*/
	std::array<ImguiRenderData, config::NUM_RENDER_THREAD_BUFFERED_FRAMES> m_imguiRenderDataBuffer;
};

}// end namespace ph::editor
