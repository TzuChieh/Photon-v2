#pragma once

#include "App/Module/RenderModule.h"

#include <Math/TVector2.h>

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
	void initializeImgui();
	void terminateImgui();
	void setFrameBufferSizePx(const math::Vector2S& sizePx);

	math::TVector2<uint32> m_frameBufferSizePx;
};

inline std::string ImguiRenderModule::getName() const
{
	return "imgui";
}

}// end namespace ph::editor
