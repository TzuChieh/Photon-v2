#pragma once

#include <Utility/IMoveOnly.h>
#include <Common/memory.h>
#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor
{

class GraphicsFrameArena final : private IMoveOnly
{
public:
	GraphicsFrameArena();
	explicit GraphicsFrameArena(std::size_t blockSizeHintInBytes);
	GraphicsFrameArena(GraphicsFrameArena&& other) noexcept;
	GraphicsFrameArena& operator = (GraphicsFrameArena&& rhs) noexcept;

	template<typename T, typename... Args>
	T* make(Args&&... args);

	template<typename T>
	TSpan<T> makeArray();

private:
	std::byte* allocRaw(std::size_t numBytes, std::size_t alignmentInBytes);

	TAlignedMemoryUniquePtr<std::byte> m_memoryBlock;
	std::byte* m_ptrInBlock;
	std::size_t m_blockSizeInBytes;
	std::size_t m_remainingBytesInBlock;
};

}// end namespace ph::editor

#include "RenderCore/Memory/GraphicsFrameArena.ipp"
