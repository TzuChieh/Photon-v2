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

	inline virtual void onFrameBufferSizeChanged(
		const math::Vector2S& oldSizePx,
		const math::Vector2S& newSizePx)
	{}

private:
};

}// end namespace ph::editor
