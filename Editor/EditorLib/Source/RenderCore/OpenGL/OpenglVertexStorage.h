#pragma once

#include "RenderCore/GHIVertexStorage.h"
#include "ThirdParty/glad2.h"

#include <cstddef>

namespace ph::editor
{

class OpenglVertexStorage : public GHIVertexStorage
{
public:
	OpenglVertexStorage(
		const GHIInfoVertexGroupFormat& format, 
		std::size_t numVertices,
		EGHIInfoStorageUsage usage);

	~OpenglVertexStorage() override;

	void upload(
		const std::byte* rawVertexData,
		std::size_t numBytes) override;

	NativeHandle getNativeHandle() override;

	std::size_t numBytes() const;

private:
	GLuint m_vboID;
	std::size_t m_numVertices;
};

}// end namespace ph::editor
