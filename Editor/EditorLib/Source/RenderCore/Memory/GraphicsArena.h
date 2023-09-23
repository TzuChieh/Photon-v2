#pragma once

#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor::ghi
{

class GraphicsMemoryManager;
class GraphicsMemoryBlock;

/*! @brief Graphics memory allocation helper.
The arena is allocating lazily, i.e., constructing and copying an arena instance won't allocate
any memory, the actual allocation occurs when memory is explicitly requested. In addition to memory
reusability, this class also provides various fallback mechanisms to ensure every memory request
can be fulfilled.
*/
class GraphicsArena final
{
public:
	enum class EType
	{
		Host,
		Device,
		RenderProducerHost,
		RenderProducerDevice,
	};

	GraphicsArena(GraphicsMemoryManager* manager, EType type);

	/*! @brief Make an object of type `T` as if by calling its constructor with `Args`.
	@tparam T Type for the object created. Must be trivially destructible.
	@param args Arguments for calling the constructor of @p T.
	@return Pointer to the created object. Never null.
	*/
	template<typename T, typename... Args>
	T* make(Args&&... args);

	/*! @brief Make an array of default constructed objects of type `T`.
	@tparam T Type for the objects created. Must be default constructible and trivially destructible.
	@return Span of the created objects. Never empty.
	*/
	template<typename T>
	TSpan<T> makeArray(std::size_t arraySize);

private:
	/*!
	@return Pointer to the newly allocated block. Never null.
	*/
	GraphicsMemoryBlock* allocNextBlock();

	GraphicsMemoryBlock* allocCustomBlock(std::size_t blockSize, std::size_t blockAlignment);

	GraphicsMemoryManager* m_manager;
	GraphicsMemoryBlock* m_memoryBlock;
	EType m_type;
};

}// end namespace ph::editor::ghi

#include "RenderCore/Memory/GraphicsArena.ipp"
