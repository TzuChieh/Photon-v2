#pragma once

#include "RenderCore/GHIStorage.h"

#include <cstddef>

namespace ph::editor
{

class GHIIndexStorage : public GHIStorage
{
public:
	GHIIndexStorage(EGHIInfoStorageElement indexType, EGHIInfoStorageUsage usage);
	~GHIIndexStorage() override;

	virtual std::size_t numIndices() const = 0;

	void upload(
		const std::byte* rawVertexData,
		std::size_t numBytes) override = 0;

	EGHIInfoStorageElement getIndexType() const;

protected:
	bool isIntegerIndexType() const;

private:
	EGHIInfoStorageElement m_indexType;
};

inline EGHIInfoStorageElement GHIIndexStorage::getIndexType() const
{
	return m_indexType;
}

}// end namespace ph::editor
