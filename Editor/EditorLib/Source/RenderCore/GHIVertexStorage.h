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
	/*! Number of bytes to offset from the start of the vertex data. */
	std::size_t strideOffset;

	/*! Number of bytes to step over to reach the next attribute data. Effectively added to `strideOffset` 
	i times for the i-th attribute.
	*/
	uint16 strideSize : 10;

	uint16 numElements : 2;
	uint16 shouldNormalize : 1;

	EGHIInfoStorageElement elementType;

	/*! @brief Empty attribute.
	*/
	GHIInfoVertexAttributeLocator();

	bool isEmpty() const;
	std::size_t numAttributeBytes() const;
};

class GHIInfoVertexGroupFormat final
{
public:
	inline constexpr static uint8 MAX_ATTRIBUTES = 8;

	std::array<GHIInfoVertexAttributeLocator, MAX_ATTRIBUTES> attributes;

	GHIInfoVertexGroupFormat();

	std::size_t numGroupBytes() const;
};

class GHIVertexStorage : public GHIStorage
{
public:
	GHIVertexStorage(const GHIInfoVertexGroupFormat& format, EGHIInfoStorageUsage usage);
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
	return elementType == EGHIInfoStorageElement::Empty;
}

inline const GHIInfoVertexGroupFormat& GHIVertexStorage::getFormat() const
{
	return m_format;
}

}// end namespace ph::editor
