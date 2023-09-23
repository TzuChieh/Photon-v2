#pragma once

#include "RenderCore/VertexStorage.h"

#include "ThirdParty/glad2.h"

#include <cstddef>

namespace ph::editor::ghi
{

class OpenglVertexStorage : public VertexStorage
{
public:
	OpenglVertexStorage(
		const VertexGroupFormatInfo& format, 
		std::size_t numVertices,
		EStorageUsage usage);

	~OpenglVertexStorage() override;

	std::size_t numVertices() const override;

	void upload(
		const std::byte* rawVertexData,
		std::size_t numBytes) override;

	NativeHandle getNativeHandle() override;

	std::size_t numBytes() const;

private:
	GLuint m_vboID;
	std::size_t m_numVertices;
};

inline std::size_t OpenglVertexStorage::numVertices() const
{
	return m_numVertices;
}

}// end namespace ph::editor::ghi
