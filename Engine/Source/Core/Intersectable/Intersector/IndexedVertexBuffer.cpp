#include "Core/Intersectable/Intersector/IndexedVertexBuffer.h"
#include "Common/logging.h"
#include "Common/assertion.h"

#include <exception>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(IndexedVertexBuffer, Core);

IndexedVertexBuffer::Entry::Entry() :
	element     (EVertexElement::VE_Float32),
	numElements (0),
	isNormalized(0),
	strideOffset(0)
{
	PH_ASSERT(isEmpty());
}

IndexedVertexBuffer::IndexedVertexBuffer() :
	m_entries       (),
	m_byteBuffer    (nullptr),
	m_byteBufferSize(0),
	m_strideSize    (0)
{
	PH_ASSERT(!isAllocated());
}

void IndexedVertexBuffer::setEntry(
	const EVertexAttribute attribute,
	const EVertexElement   element,
	const std::size_t      numElements,
	const bool             isNormalized)
{
	const auto entryIndex = static_cast<std::size_t>(attribute);
	if(entryIndex >= m_entries.size())
	{
		throw std::invalid_argument("Invalid entry index. Check the input attribute.");
	}

	Entry inputEntry;

	if(static_cast<std::size_t>(element) < static_cast<std::size_t>(EVertexElement::NUM))
	{
		inputEntry.element = element;
	}
	else
	{
		throw std::invalid_argument("Invalid vertex element type.");
	}

	if(numElements <= 3)
	{
		if(element == EVertexElement::VE_OctahedralUnitVec32 || element == EVertexElement::VE_OctahedralUnitVec24)
		{
			if(numElements != 0 && numElements != 2)
			{
				PH_LOG(IndexedVertexBuffer, 
					"Octahedral unit vector is defined to have 2 elements. The specified number ({}) is ignored.",
					numElements);
			}

			inputEntry.numElements = 2;
		}
		else
		{
			inputEntry.numElements = static_cast<uint8>(numElements);
		}	
	}
	else
	{
		throw std::invalid_argument("Cannot handle more than 3 elements in a single attribute.");
	}
	
	inputEntry.isNormalized = isNormalized ? 1 : 0;

	m_entries[entryIndex] = inputEntry;
}

void IndexedVertexBuffer::allocate(const std::size_t numVertices)
{
	// Update stride offset in the entries

	StrideSize currentStrideSize = 0;
	for(Entry& entry : m_entries)
	{
		if(entry.isEmpty())
		{
			continue;
		}

		entry.strideOffset = currentStrideSize;

		switch(entry.element)
		{
		case EVertexElement::VE_Float32:
		case EVertexElement::VE_Int32:
			currentStrideSize += 4 * entry.numElements;
			break;

		case EVertexElement::VE_Float16:
		case EVertexElement::VE_Int16:
			currentStrideSize += 2 * entry.numElements;
			break;

		case EVertexElement::VE_OctahedralUnitVec32:
			currentStrideSize += 4;
			break;

		case EVertexElement::VE_OctahedralUnitVec24:
			currentStrideSize += 3;
			break;

		default:
			throw std::invalid_argument("Cannot allocate storage for invalid vertex element type.");
			break;
		}
	}
	
	m_strideSize = currentStrideSize;

	// Allocate storage for the entries

	// Possibly clear existing buffer first to reduce memory usage
	m_byteBuffer = nullptr;

	m_byteBufferSize = numVertices * m_strideSize;
	m_byteBuffer = std::make_unique<std::byte[]>(m_byteBufferSize);

	if(m_byteBufferSize == 0)
	{
		PH_LOG_WARNING(IndexedVertexBuffer, "Allocated buffer with 0 size.");
	}
}

math::Vector3R IndexedVertexBuffer::getAttribute(const EVertexAttribute attribute, const std::size_t index) const
{
	const auto entryIndex = static_cast<std::size_t>(attribute);
	PH_ASSERT_LT(entryIndex, m_entries.size());

	const Entry& entry = m_entries[entryIndex];
	if(entry.isEmpty())
	{
		return math::Vector3R(0);
	}

	const auto byteIndex = index * m_strideSize + entry.strideOffset;
	PH_ASSERT_LT(byteIndex, m_byteBufferSize);


}

}// end namespace ph
