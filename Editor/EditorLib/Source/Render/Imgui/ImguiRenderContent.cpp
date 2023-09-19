#include "Render/Imgui/ImguiRenderContent.h"
#include "Render/UpdateContext.h"
#include "EditorCore/Thread/Threads.h"
#include "RenderCore/GHIThreadCaller.h"
#include "RenderCore/GraphicsContext.h"
#include "RenderCore/GHI.h"

#include "ThirdParty/glad2.h"

#include <Common/assertion.h>
#include <Common/profiling.h>
#include <Utility/utility.h>

#include <algorithm>

namespace ph::editor::render
{

ImguiRenderContent::ImguiRenderContent()

	: IDynamicSceneResource()

	, m_sharedRenderData()
	, m_numAvailableRenderData(0)
{}

void ImguiRenderContent::update(const UpdateContext& ctx)
{
	// TODO: properly get current viewport size (is it necessary or imgui handles it?)
}

void ImguiRenderContent::createGHICommands(GHIThreadCaller& caller)
{
	PH_PROFILE_SCOPE();

	caller.add(
		[](GraphicsContext& ctx)
		{
			ctx.getGHI().rawCommand<EGraphicsAPI::OpenGL>(
				[]()
				{
					ImGui_ImplOpenGL3_NewFrame();

					//int display_w, display_h;
					//glfwGetFramebufferSize(window, &display_w, &display_h);
					glViewport(0, 0, 512, 512);
					glClearColor(0, 0, 0, 1);
					glClear(GL_COLOR_BUFFER_BIT);
				});
		});

	// Only emit GHI command if render data was actually copied to avoid blocking GHI thread
	if(m_numAvailableRenderData > 0)
	{
		caller.add(
			[this](GraphicsContext& /* ctx */)
			{
				// We never want to block GHI thread. If so, consider increase the size of shared data.
				PH_ASSERT(m_sharedRenderData.mayWaitToConsume() == false);

				m_sharedRenderData.guardedConsume(
					[](ImguiRenderData& renderData)
					{
						ImGui_ImplOpenGL3_RenderDrawData(&renderData.drawData);
					});
			});

		--m_numAvailableRenderData;
	}
}

void ImguiRenderContent::setupGHI(GHIThreadCaller& caller)
{}

void ImguiRenderContent::cleanupGHI(GHIThreadCaller& caller)
{}

auto ImguiRenderContent::getSharedRenderData() 
	-> SharedRenderData&
{
	PH_ASSERT(Threads::isOnMainThread());

	return m_sharedRenderData;
}

void ImguiRenderContent::signifyNewRenderDataIsAvailable()
{
	PH_ASSERT(Threads::isOnRenderThread());

	++m_numAvailableRenderData;
}

void ImguiRenderContent::ImguiRenderData::copyFrom(const ImDrawData& srcDrawData)
{
	PH_PROFILE_SCOPE();

	// We deep copy `ImDrawList` here, as the comment for `ImDrawData::CmdLists` says that the 
	// draw lists are owned by `ImGuiContext`, simply swap them with our buffer (which is a 
	// modification) might broke some assumptions from IMGUI framework.
	// Here we aim to minimize the amount of memory allocation & deallocation for max efficiency. 
	// May need to check IMGUI source again if the library is updated.

	const auto srcDrawListsSize = lossless_cast<std::size_t>(srcDrawData.CmdListsCount);

	// Prepare space for draw list data (only grow, for efficiency)
	if(drawListBuffer.size() < srcDrawListsSize)
	{
		drawListBuffer.resize(srcDrawListsSize, ImDrawList(nullptr));
	}
	if(drawListPtrBuffer.size() < srcDrawListsSize)
	{
		drawListPtrBuffer.resize(srcDrawListsSize, nullptr);
	}

	// Deep copy `ImGuiViewport` into our buffer, this struct is only defined in docking branch
	// NOTE: this struct contains multiple `void*` fields for IMGUI's backend implementation, 
	// watchout for threading issue as those data might get accessed concurrently here
	ownerViewportBuffer = *srcDrawData.OwnerViewport;
	ownerViewportBuffer.DrawData = &drawData;

	// Shallow copy `ImDrawData` first
	drawData = srcDrawData;

	// Set `ImDrawData::CmdLists` to our pointer buffer
	drawData.CmdLists = drawListPtrBuffer.data();

	// Set `ImDrawData::OwnerViewport` to our viewport buffer
	drawData.OwnerViewport = &ownerViewportBuffer;

	// Deep copy `ImDrawData` draw lists
	for(std::size_t listIdx = 0; listIdx < srcDrawListsSize; ++listIdx)
	{
		// Deep copy draw list into buffer first

		PH_ASSERT(srcDrawData.CmdLists && srcDrawData.CmdLists[listIdx]);
		ImDrawList& srcDrawList = *srcDrawData.CmdLists[listIdx];
		ImDrawList& dstDrawList = drawListBuffer[listIdx];

		// Note that to deep copy an `ImVector`, do not use its assignment operator as it always free &
		// allocate for all data, which is hugely inefficient.

		// TODO: use reserve_discard, and don't forget to update Size
		// Pre-allocate buffer to the same size (`ImVector::resize()` will grow only)
		dstDrawList.CmdBuffer.resize(srcDrawList.CmdBuffer.size());
		dstDrawList.IdxBuffer.resize(srcDrawList.IdxBuffer.size());
		dstDrawList.VtxBuffer.resize(srcDrawList.VtxBuffer.size());

		std::copy(
			srcDrawList.CmdBuffer.Data, 
			srcDrawList.CmdBuffer.Data + srcDrawList.CmdBuffer.Size, 
			dstDrawList.CmdBuffer.Data);
		std::copy(
			srcDrawList.IdxBuffer.Data,
			srcDrawList.IdxBuffer.Data + srcDrawList.IdxBuffer.Size,
			dstDrawList.IdxBuffer.Data);
		std::copy(
			srcDrawList.VtxBuffer.Data,
			srcDrawList.VtxBuffer.Data + srcDrawList.VtxBuffer.Size,
			dstDrawList.VtxBuffer.Data);

		dstDrawList.Flags = srcDrawList.Flags;

		// Done copying draw list into buffer, update the pointer
		drawListPtrBuffer[listIdx] = &drawListBuffer[listIdx];
	}
}

}// end namespace ph::editor::render
