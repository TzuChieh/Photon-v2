#pragma once

#include "App/Module/RenderModule.h"

namespace ph::editor
{

class ImguiRenderModule : public RenderModule
{
public:
	std::string getName() const override;
	void onAttach(const ModuleAttachmentInfo& info) override;
	void onDetach() override;
	void renderUpdate(const MainThreadRenderUpdateContext& ctx) override;

private:
};

inline std::string ImguiRenderModule::getName() const
{
	return "imgui";
}

}// end namespace ph::editor
