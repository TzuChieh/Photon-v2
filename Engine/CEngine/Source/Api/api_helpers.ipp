#pragma once

#include "Api/api_helpers.h"

#include <Utility/ByteBuffer.h>

#include <type_traits>

namespace ph
{

template<typename T>
inline T* make_array_from_buffer(std::size_t arraySize, ByteBuffer& buffer)
{
	// TODO
}

}// end namespace ph
