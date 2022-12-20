#pragma once

#include "RenderCore/GHIStorage.h"
#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>

#include <array>
#include <cstddef>

namespace ph::editor
{

class GHIInfoVertexAttributeLocator final
{
public:
	std::size_t strideOffset;
	std::size_t strideSize;
	EGHIInfoStorageElement elementType;
	uint8 numElements : 2;
	uint8 shouldNormalize : 1;

	/*! @brief Empty attribute.
	*/
	GHIInfoVertexAttributeLocator();
};

class GHIInfoVertexLayout final
{
public:
	inline constexpr static uint8 MAX_VERTEX_ATTRIBUTES = 16;

	std::array<GHIInfoVertexAttributeLocator, MAX_VERTEX_ATTRIBUTES> attributes;
};

class GHIVertexStorage : public GHIStorage
{
public:
	explicit GHIVertexStorage(const GHIInfoVertexLayout& vertexLayout);
	~GHIVertexStorage() override;

	virtual void upload(
		const std::byte* vertexData,
		std::size_t numBytes,
		EGHIInfoStorageElement elementType) = 0;

	const GHIInfoVertexLayout& getVertexLayout() const;

private:
	GHIInfoVertexLayout m_vertexLayout;
};

inline const GHIInfoVertexLayout& GHIVertexStorage::getVertexLayout() const
{
	return m_vertexLayout;
}

}// end namespace ph::editor
