#pragma once

#include "RenderCore/Storage.h"

#include <cstddef>

namespace ph::editor::ghi
{

class IndexStorage : public Storage
{
public:
	IndexStorage(EStorageElement indexType, EStorageUsage usage);
	~IndexStorage() override;

	virtual std::size_t numIndices() const = 0;

	void upload(
		const std::byte* rawVertexData,
		std::size_t numBytes) override = 0;

	EStorageElement getIndexType() const;

protected:
	bool isIntegerIndexType() const;

private:
	EStorageElement m_indexType;
};

inline EStorageElement IndexStorage::getIndexType() const
{
	return m_indexType;
}

}// end namespace ph::editor::ghi
