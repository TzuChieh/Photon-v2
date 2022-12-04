#pragma once

#include "App/Module/AppModule.h"

#include <Math/TVector2.h>

namespace ph::editor
{

class ModuleAttachmentInfo;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

class RenderModule : public AppModule
{
public:
	std::string getName() const override = 0;

	/*!
	Called before a frame is going to be rendered. Called on main thread.
	*/
	virtual void renderUpdate(const MainThreadRenderUpdateContext& ctx) = 0;

	/*!
	Called when a frame is being rendered. Called on main thread.
	*/
	virtual void createRenderCommands(RenderThreadCaller& caller) = 0;

	/*!
	Optionally perform initialization and termination operations on render thread.
	Default implementation does nothing. Called on main thread.
	*/
	///@{
	virtual void createSetupRenderCommands(RenderThreadCaller& caller);
	virtual void createCleanupRenderCommands(RenderThreadCaller& caller);
	///@}

	void onAttach(const ModuleAttachmentInfo& info) override = 0;
	void onDetach() override = 0;

private:
};

}// end namespace ph::editor
