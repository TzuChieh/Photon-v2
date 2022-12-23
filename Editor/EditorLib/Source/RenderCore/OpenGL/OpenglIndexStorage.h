#pragma once

#include "RenderCore/GHIIndexStorage.h"
#include "ThirdParty/glad2.h"

#include <cstddef>

namespace ph::editor
{

class OpenglIndexStorage : public GHIIndexStorage
{
public:
	OpenglIndexStorage(
		EGHIInfoStorageElement indexType,
		std::size_t numIndices,
		EGHIInfoStorageUsage usage);

	~OpenglIndexStorage() override;

	void upload(
		const std::byte* rawIndexData,
		std::size_t numBytes) override;

	NativeHandle getNativeHandle() override;

	std::size_t numBytes() const;

private:
	GLuint m_indexBufferID;
	std::size_t m_numIndices;
};

}// end namespace ph::editor
