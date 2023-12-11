#pragma once

#include "ph_c_core_types.h"

namespace ph { class PictureMeta; }

namespace ph
{

PictureMeta make_picture_meta(const PhFrameSaveInfo& saveInfo);

}// end namespace ph
