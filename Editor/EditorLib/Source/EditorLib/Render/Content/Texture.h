#pragma once

#include "EditorLib/Render/Content/fwd.h"
#include "EditorLib/RenderCore/ghi_fwd.h"
#include "EditorLib/RenderCore/ghi_infos.h"

namespace ph::editor::render
{

class Texture final
{
public:
	ghi::TextureHandle handle;
	ghi::TextureDesc desc;
};

}// end namespace ph::editor::render
