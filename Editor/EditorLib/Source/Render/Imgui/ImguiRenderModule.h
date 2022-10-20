#pragma once

#include "Render/RenderModule.h"

namespace ph::editor
{

class ImguiRenderModule : public RenderModule
{
public:
	void onAttach(const ModuleAttachmentInfo& info) override;
	void onDetach() override;

private:
};

}// end namespace ph::editor
