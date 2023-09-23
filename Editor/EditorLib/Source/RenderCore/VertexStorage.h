#pragma once

#include "RenderCore/Storage.h"
#include "RenderCore/ghi_enums.h"
#include "RenderCore/ghi_infos.h"

#include <cstddef>

namespace ph::editor::ghi
{

class VertexStorage : public Storage
{
public:
	VertexStorage(const VertexGroupFormatInfo& format, EStorageUsage usage);
	~VertexStorage() override;

	virtual std::size_t numVertices() const = 0;

	void upload(
		const std::byte* rawVertexData,
		std::size_t numBytes) override = 0;

	const VertexGroupFormatInfo& getFormat() const;

private:
	VertexGroupFormatInfo m_format;
};

inline bool VertexAttributeLocatorInfo::isEmpty() const
{
	return elementType == EStorageElement::Empty;
}

inline const VertexGroupFormatInfo& VertexStorage::getFormat() const
{
	return m_format;
}

}// end namespace ph::editor::ghi
