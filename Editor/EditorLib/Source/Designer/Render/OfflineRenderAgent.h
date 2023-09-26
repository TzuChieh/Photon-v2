#pragma once

#include "Designer/Render/RenderAgent.h"

#include <SDL/sdl_interface.h>

namespace ph::editor::render { class OfflineRenderer; }

namespace ph::editor
{

class OfflineRenderAgent : public RenderAgent
{
public:
	using Base = RenderAgent;

	RenderConfig getRenderConfig() const override;

	void renderInit(RenderThreadCaller& caller);
	void renderUninit(RenderThreadCaller& caller);
	void update(const MainThreadUpdateContext& ctx);
	void renderUpdate(const MainThreadRenderUpdateContext& ctx);

private:
	render::OfflineRenderer* m_renderer = nullptr;

	// SDL-binded fields
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
