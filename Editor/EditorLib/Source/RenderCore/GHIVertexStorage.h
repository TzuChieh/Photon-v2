#pragma once

#include "RenderCore/GHIStorage.h"
#include "RenderCore/ghi_enums.h"

#include "Common/primitive_type.h"

#include <array>

namespace ph::editor
{

class GHIInfoVertexAttributeLocator final
{
public:
	EGHIInfoStorageElement elementType;


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
	~GHIVertexStorage() override;
};

}// end namespace ph::editor
