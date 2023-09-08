#pragma once

#include <Utility/TSpan.h>

#include <cstddef>

namespace ph::editor
{

class GraphicsMemoryManager;
class GraphicsMemoryBlock;

class GraphicsArena final
{
public:
	enum class EType
	{
		Host,
		Device,
		RendererHost,
		RendererDevice,
	};

	GraphicsArena(GraphicsMemoryManager* manager, EType type);

	/*! @brief Make an object of type `T` as if by calling its constructor with `Args`.
	@tparam T Type for the object created. Must be trivially destructible.
	@param args Arguments for calling the constructor of @p T.
	@return Pointer to the created object. Null if not enough storage is left.
	*/
	template<typename T, typename... Args>
	T* make(Args&&... args);

	/*! @brief Make an array of default constructed objects of type `T`.
	@tparam T Type for the objects created. Must be default constructible and trivially destructible.
	@return Span of the created objects. Empty if not enough storage is left.
	*/
	template<typename T>
	TSpan<T> makeArray(std::size_t arraySize);

private:
	/*!
	@return Pointer to the newly allocated block. Never null.
	*/
	GraphicsMemoryBlock* allocNextBlock();

	GraphicsMemoryManager* m_manager;
	GraphicsMemoryBlock* m_memoryBlock;
	EType m_type;
};

}// end namespace ph::editor

#include "RenderCore/Memory/GraphicsArena.ipp"
