#pragma once

#include "Designer/Render/RenderAgent.h"

#include <SDL/sdl_interface.h>
#include <DataIO/sdl_picture_file_type.h>

namespace ph::editor::render { class OfflineRenderer; }

namespace ph::editor
{

class OfflineRenderAgent : public RenderAgent
{
public:
	using Base = RenderAgent;

	RenderConfig getRenderConfig() const override;
	void render(RenderConfig config) override;

	void renderInit(RenderThreadCaller& caller) override;
	void renderUninit(RenderThreadCaller& caller) override;
	void update(const MainThreadUpdateContext& ctx) override;
	void renderUpdate(const MainThreadRenderUpdateContext& ctx) override;

private:
	render::OfflineRenderer* m_renderer = nullptr;

	// SDL-binded fields
	EPictureFile m_outputFileFormat;
	bool m_useCopiedScene;
	bool m_enableStatsRequest;
	bool m_enablePeekingFrame;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<OfflineRenderAgent>)
	{
		ClassType clazz("offline-render-agent");
		clazz.docName("Offline Render Agent");
		clazz.description("Controls offline rendering.");
		clazz.baseOn<RenderAgent>();

		TSdlEnumField<OwnerType, EPictureFile> outputFileFormat("output-file-format", &OwnerType::m_outputFileFormat);
		outputFileFormat.description("File format of the rendered output.");
		outputFileFormat.defaultTo(EPictureFile::PNG);
		clazz.addField(outputFileFormat);

		TSdlBool<OwnerType> useCopiedScene("use-copied-scene", &OwnerType::m_useCopiedScene);
		useCopiedScene.description(
			"Whether to use a copied scene for rendering.");
		useCopiedScene.defaultTo(true);
		clazz.addField(useCopiedScene);

		TSdlBool<OwnerType> enableStatsRequest("enable-stats-request", &OwnerType::m_enableStatsRequest);
		enableStatsRequest.description(
			"Whether to enable the retrieval of rendering statistics.");
		enableStatsRequest.defaultTo(true);
		clazz.addField(enableStatsRequest);

		TSdlBool<OwnerType> enablePeekingFrame("enable-peeking-frame", &OwnerType::m_enablePeekingFrame);
		enablePeekingFrame.description(
			"Whether to enable the retrieval of intermediate rendering result.");
		enablePeekingFrame.defaultTo(true);
		clazz.addField(enablePeekingFrame);

		return clazz;
	}
};

}// end namespace ph::editor
