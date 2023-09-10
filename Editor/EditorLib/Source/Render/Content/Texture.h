#pragma once

#include "Render/Content/fwd.h"
#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_infos.h"

namespace ph::editor::render
{

class Texture final
{
public:
	GHITextureHandle handle;
	GHIInfoTextureDesc desc;
};

}// end namespace ph::editor::render
