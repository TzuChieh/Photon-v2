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
		EGHIStorageElement indexType,
		std::size_t numIndices,
		EGHIStorageUsage usage);

	~OpenglIndexStorage() override;

	std::size_t numIndices() const override;

	void upload(
		const std::byte* rawIndexData,
		std::size_t numBytes) override;

	NativeHandle getNativeHandle() override;

	std::size_t numBytes() const;

private:
	GLuint m_iboID;
	std::size_t m_numIndices;
};

inline std::size_t OpenglIndexStorage::numIndices() const
{
	return m_numIndices;
}

}// end namespace ph::editor
