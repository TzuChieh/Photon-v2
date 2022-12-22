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
	/*! Number of bytes to step over to reach the start of the attribute. Effectively added to
	`GHIInfoVertexGroup::groupOffset` for the attribute.
	*/
	uint16 inGroupOffset : 10;

	uint16 numElements : 2;
	uint16 shouldNormalize : 1;

	EGHIInfoStorageElement elementType;

	/*! @brief Empty attribute.
	*/
	GHIInfoVertexAttributeLocator();

	bool isEmpty() const;
	std::size_t numAttributeBytes() const;
};

class GHIInfoVertexGroup final
{
public:
	inline constexpr static uint8 MAX_ATTRIBUTES = 8;

	/*! Number of bytes to offset from the start of the vertex group. */
	std::size_t groupOffset;

	/*! Indices of the attributes that are in this group. */
	std::array<uint8, MAX_ATTRIBUTES> attributeIndices;

	uint8 numAttributes;

	/*! @brief Empty group.
	*/
	GHIInfoVertexGroup();

	bool isEmpty() const;
};

class GHIInfoVertexLayout final
{
public:
	inline constexpr static uint8 MAX_VERTEX_ATTRIBUTES = 16;
	inline constexpr static uint8 MAX_VERTEX_GROUPS = 4;

	std::array<GHIInfoVertexAttributeLocator, MAX_VERTEX_ATTRIBUTES> attributes;
	std::array<GHIInfoVertexGroup, MAX_VERTEX_GROUPS> groups;

	GHIInfoVertexLayout();

	std::size_t numGroupBytes(std::size_t groupIndex) const;
	std::size_t numVertexBytes() const;
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

inline bool GHIInfoVertexAttributeLocator::isEmpty() const
{
	return elementType == EGHIInfoStorageElement::Empty;
}

inline bool GHIInfoVertexGroup::isEmpty() const
{
	return numAttributes == 0;
}

inline const GHIInfoVertexLayout& GHIVertexStorage::getVertexLayout() const
{
	return m_vertexLayout;
}

}// end namespace ph::editor
