#pragma once

#include "RenderCore/GHIStorage.h"

#include <Common/primitive_type.h>

#include <array>
#include <span>
#include <memory>
#include <vector>
#include <cstddef>

namespace ph::editor
{

class GHIVertexStorage;

class GHIInfoMeshVertexLayout final
{
public:
	inline constexpr static uint8 MAX_ATTRIBUTE_BINDINGS = 16;

	struct AttributeBinding final
	{
		uint8 storageIndex;
		uint8 attributeIndex;
	};

	/*! Indices for the attributes referenced by the mesh. */
	std::array<AttributeBinding, MAX_ATTRIBUTE_BINDINGS> attributeBindings;

	uint8 numAttributes;

	/*! @brief Empty layout.
	*/
	GHIInfoMeshVertexLayout();

	bool isEmpty() const;
};

class GHIMeshStorage : public GHIStorage
{
public:
	GHIMeshStorage(
		const GHIInfoMeshVertexLayout& layout,
		std::span<std::shared_ptr<GHIVertexStorage>> vertexStorages);

	const GHIInfoMeshVertexLayout& getLayout() const;
	const GHIVertexStorage& getStorage(std::size_t storageIndex) const;
	GHIVertexStorage& getStorage(std::size_t storageIndex);
	std::shared_ptr<GHIVertexStorage> getStorageResource(std::size_t storageIndex) const;

private:
	GHIInfoMeshVertexLayout m_layout;
	std::vector<std::shared_ptr<GHIVertexStorage>> m_vertexStorages;
};

}// end namespace ph::editor
