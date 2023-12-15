#pragma once

#include "ph_c_core_types.h"

#include <cstddef>

namespace ph { class PictureMeta; }
namespace ph { class ByteBuffer; }

namespace ph
{

PictureMeta make_picture_meta(const PhFrameSaveInfo& saveInfo);

template<typename T>
T* make_array_from_buffer(std::size_t arraySize, ByteBuffer& buffer);

}// end namespace ph

#include "Api/api_helpers.ipp"
