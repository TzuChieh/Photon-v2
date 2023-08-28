#pragma once

#include "RenderCore/GHIStorage.h"

#include <cstddef>

namespace ph::editor
{

class GHIIndexStorage : public GHIStorage
{
public:
	GHIIndexStorage(EGHIStorageElement indexType, EGHIStorageUsage usage);
	~GHIIndexStorage() override;

	virtual std::size_t numIndices() const = 0;

	void upload(
		const std::byte* rawVertexData,
		std::size_t numBytes) override = 0;

	EGHIStorageElement getIndexType() const;

protected:
	bool isIntegerIndexType() const;

private:
	EGHIStorageElement m_indexType;
};

inline EGHIStorageElement GHIIndexStorage::getIndexType() const
{
	return m_indexType;
}

}// end namespace ph::editor
