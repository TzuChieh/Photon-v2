#pragma once

#include "RenderCore/CustomRenderContent.h"
#include "editor_lib_config.h"
#include "ThirdParty/DearImGui.h"

#include <Utility/Concurrent/TSPSCCircularBuffer.h>

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

	/*
	Can only be called during render command generation (on main thread).
	*/
	void copyNewDrawDataFromMainThread(const ImDrawData& srcDrawData, std::size_t mainThreadFrameCycleIndex);

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
	[2] You only need the ImDrawData struct for rendering
	    https://twitter.com/ocornut/status/973093439666520064
	[3] https://twitter.com/ocornut/status/975096904446021633
	*/
	std::array<ImguiRenderData, config::NUM_RENDER_THREAD_BUFFERED_FRAMES> m_imguiRenderDataBuffer;
	TSPSCCircularBuffer<ImguiRenderData, config::NUM_RENDER_THREAD_BUFFERED_FRAMES> m_test;

	std::size_t m_currentFrameCycleIndex;
};

}// end namespace ph::editor
