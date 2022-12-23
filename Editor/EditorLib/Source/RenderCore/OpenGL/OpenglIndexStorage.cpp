#include "RenderCore/OpenGL/OpenglIndexStorage.h"

#include <Common/logging.h>

namespace ph::editor
{

OpenglIndexStorage::OpenglIndexStorage(
	const EGHIInfoStorageElement indexType,
	const std::size_t numIndices,
	const EGHIInfoStorageUsage usage)

	: GHIIndexStorage(indexType, usage)

	, m_iboID(0)
	, m_numIndices(numIndices)
{
	if(!isIntegerIndexType())
	{
		PH_DEFAULT_LOG_ERROR(
			"[OpenglIndexStorage] non-integer type used as index type");
	}
}

OpenglIndexStorage::~OpenglIndexStorage()
{
	glDeleteBuffers(1, &m_iboID);
}

void OpenglIndexStorage::upload(
	const std::byte* const rawIndexData,
	const std::size_t inNumBytes)
{
	PH_ASSERT(rawIndexData);
	PH_ASSERT_NE(m_iboID, 0);

	// The input data must be for the entire index buffer--same number of total bytes
	PH_ASSERT_EQ(numBytes(), inNumBytes);

	if(getUsage() == EGHIInfoStorageUsage::Static)
	{
		glNamedBufferStorage(
			m_iboID,
			lossless_cast<GLsizeiptr>(numBytes()),
			rawIndexData,
			0);
	}
	else
	{
		glNamedBufferData(
			m_iboID,
			lossless_cast<GLsizeiptr>(numBytes()),
			rawIndexData,
			GL_DYNAMIC_DRAW);
	}
}

auto OpenglIndexStorage::getNativeHandle()
-> NativeHandle
{
	if(m_iboID != 0)
	{
		return static_cast<uint64>(m_iboID);
	}
	else
	{
		return std::monostate{};
	}
}

std::size_t OpenglIndexStorage::numBytes() const
{
	return num_bytes(getIndexType()) * m_numIndices;
}

}// end namespace ph::editor
