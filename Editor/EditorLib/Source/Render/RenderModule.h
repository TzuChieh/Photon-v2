#pragma once

#include <Math/TVector2.h>

namespace ph::editor
{

class ModuleAttachmentInfo;
class MainThreadRenderUpdateContext;

class RenderModule
{
public:
	virtual ~RenderModule();

	virtual void onAttach(const ModuleAttachmentInfo& info) = 0;
	virtual void onDetach() = 0;
	virtual void renderUpdate(const MainThreadRenderUpdateContext& ctx) = 0;

private:
};

}// end namespace ph::editor
