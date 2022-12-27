#pragma once

#include "App/Module/RenderModule.h"

namespace ph::editor
{

class EditorDebugRenderModule : public RenderModule
{
public:
	EditorDebugRenderModule();

	std::string getName() const override;
	void onAttach(const ModuleAttachmentInfo& info) override;
	void onDetach() override;
	void renderUpdate(const MainThreadRenderUpdateContext& ctx) override;
	void createRenderCommands(RenderThreadCaller& caller) override;
	/*void createSetupRenderCommands(RenderThreadCaller& caller) override;
	void createCleanupRenderCommands(RenderThreadCaller& caller) override;*/

private:
	
};

inline std::string EditorDebugRenderModule::getName() const
{
	return "editor-debug";
}

}// end namespace ph::editor
