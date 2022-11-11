#pragma once

#include "App/Module/AppModule.h"

#include <Math/TVector2.h>

namespace ph::editor
{

class ModuleAttachmentInfo;
class MainThreadRenderUpdateContext;

class RenderModule : public AppModule
{
public:
	std::string getName() const override = 0;

	virtual void renderUpdate(const MainThreadRenderUpdateContext& ctx) = 0;

	void onAttach(const ModuleAttachmentInfo& info) override;
	void onDetach() override;

private:
};

}// end namespace ph::editor
