#include "Core/Intersectable/Intersector/IndexedVertexBuffer.h"
#include "Common/logging.h"

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
	// TODO



}

void IndexedVertexBuffer::allocate()
{
	// Update stride offset in the entries

	// TODO

}

}// end namespace ph
