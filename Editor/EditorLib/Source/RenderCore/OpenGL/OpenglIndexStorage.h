#pragma once

#include "RenderCore/IndexStorage.h"

#include "ThirdParty/glad2.h"

#include <cstddef>

namespace ph::editor::ghi
{

class OpenglIndexStorage : public IndexStorage
{
public:
	OpenglIndexStorage(
		EStorageElement indexType,
		std::size_t numIndices,
		EStorageUsage usage);

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

}// end namespace ph::editor::ghi
