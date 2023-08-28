#pragma once

#include "RenderCore/GHIStorage.h"
#include "RenderCore/ghi_enums.h"
#include "RenderCore/ghi_infos.h"

#include <cstddef>

namespace ph::editor
{

class GHIVertexStorage : public GHIStorage
{
public:
	GHIVertexStorage(const GHIInfoVertexGroupFormat& format, EGHIStorageUsage usage);
	~GHIVertexStorage() override;

	virtual std::size_t numVertices() const = 0;

	void upload(
		const std::byte* rawVertexData,
		std::size_t numBytes) override = 0;

	const GHIInfoVertexGroupFormat& getFormat() const;

private:
	GHIInfoVertexGroupFormat m_format;
};

inline bool GHIInfoVertexAttributeLocator::isEmpty() const
{
	return elementType == EGHIStorageElement::Empty;
}

inline const GHIInfoVertexGroupFormat& GHIVertexStorage::getFormat() const
{
	return m_format;
}

}// end namespace ph::editor
