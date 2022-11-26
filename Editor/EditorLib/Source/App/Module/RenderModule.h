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
	Called on main thread.
	*/
	virtual void renderUpdate(const MainThreadRenderUpdateContext& ctx) = 0;

	/*!
	Called on main thread.
	*/
	virtual void createRenderCommands(RenderThreadCaller& caller) = 0;

	void onAttach(const ModuleAttachmentInfo& info) override = 0;
	void onDetach() override = 0;

private:
};

}// end namespace ph::editor
