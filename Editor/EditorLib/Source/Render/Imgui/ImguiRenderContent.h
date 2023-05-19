#pragma once

#include "Render/CustomRenderContent.h"
#include "editor_lib_config.h"

#include "ThirdParty/DearImGui.h"

#include <Utility/Concurrent/TSPSCRingBuffer.h>

#include <array>
#include <cstddef>
#include <vector>

namespace ph::editor
{

class ImguiRenderContent : public CustomRenderContent
{
public:
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
	For N-buffered rendering, size of the ring buffer is set to N + 1 for it to be wait-free.
	Think of unbuffered rendering (render thread buffer size = 1) and ring buffer size = 1, 
	main thread will unblock if render thread just finished its work and GHI thread just started 
	its work. Thus, main thread might modify some data while GHI thread attempts to read it, 
	resulting in read/write block. For N > 1, drawing a timeline for the threads might help to
	verify the correct size for a wait-free shared buffer.
	*/
	using SharedRenderData = TSPSCRingBuffer<
		ImguiRenderData, 
		config::NUM_RENDER_THREAD_BUFFERED_FRAMES + 1>;

public:
	ImguiRenderContent();

	void update(const RenderThreadUpdateContext& ctx) override;
	void createGHICommands(GHIThreadCaller& caller) override;
	void setupGHI(GHIThreadCaller& caller) override;
	void cleanupGHI(GHIThreadCaller& caller) override;

	/*
	Can only be called during render update and command generation (on main thread).
	*/
	SharedRenderData& getSharedRenderData();

	void signifyNewRenderDataIsAvailable();

private:
	/*!
	Allocating an array of IMGUI render data is the core of N-buffered rendering, where N is the 
	number of buffered frames. This enables rendering IMGUI from a separate thread (in our case the 
	GHI thread) without conflicting concurrent access to the `ImDrawData` on main thread (the one 
	obtained from `ImGui::GetDrawData()`). `SharedRenderData` generalizes this idea into a shared 
	(ring) buffer and helps to manage concurrent access to it.

	References: 
	[1] Add a helper to simplify and optimize backing up the render frame
	    https://github.com/ocornut/imgui/pull/2433
	[2] You only need the ImDrawData struct for rendering
	    https://twitter.com/ocornut/status/973093439666520064
	[3] https://twitter.com/ocornut/status/975096904446021633
	*/
	SharedRenderData m_sharedRenderData;

	int m_numAvailableRenderData;
};

}// end namespace ph::editor
