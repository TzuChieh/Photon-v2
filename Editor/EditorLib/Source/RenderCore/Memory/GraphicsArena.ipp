#pragma once

#include "RenderCore/Memory/GraphicsArena.h"
#include "RenderCore/Memory/GraphicsMemoryBlock.h"
#include "RenderCore/ghi_exceptions.h"

namespace ph::editor
{

template<typename T, typename... Args>
inline T* GraphicsArena::make(Args&&... args)
{

}

template<typename T>
inline TSpan<T> GraphicsArena::makeArray(const std::size_t arraySize)
{

}

}// end namespace ph::editor
