#pragma once

#include <Math/TVector2.h>

namespace ph::editor
{

class ModuleAttachmentInfo;

class RenderModule
{
public:
	inline virtual ~RenderModule() = default;

	virtual void onAttach(const ModuleAttachmentInfo& info) = 0;
	virtual void onDetach() = 0;
	virtual void renderUpdate() = 0;

private:
};

}// end namespace ph::editor
