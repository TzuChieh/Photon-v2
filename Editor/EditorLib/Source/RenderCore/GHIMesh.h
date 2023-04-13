#pragma once

#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Utility/TSpan.h>

#include <array>
#include <memory>
#include <vector>
#include <cstddef>

namespace ph::editor
{

class GHIVertexStorage;
class GHIIndexStorage;

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

class GHIMesh
{
public:
	GHIMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages);

	GHIMesh(
		const GHIInfoMeshVertexLayout& layout,
		TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
		const std::shared_ptr<GHIIndexStorage>& indexStorage);

	virtual ~GHIMesh();

	virtual void bind() = 0;

	const GHIInfoMeshVertexLayout& getLayout() const;

	std::size_t numVertexStorages() const;
	const GHIVertexStorage& getVertexStorage(std::size_t storageIndex) const;
	GHIVertexStorage& getVertexStorage(std::size_t storageIndex);
	std::shared_ptr<GHIVertexStorage> getVertexStorageResource(std::size_t storageIndex) const;

	bool hasIndexStorage() const;
	const GHIIndexStorage& getIndexStorage() const;
	GHIIndexStorage& getIndexStorage();
	std::shared_ptr<GHIIndexStorage> getIndexStorageResource() const;

private:
	GHIInfoMeshVertexLayout m_layout;
	std::vector<std::shared_ptr<GHIVertexStorage>> m_vertexStorages;
	std::shared_ptr<GHIIndexStorage> m_indexStorage;
};

inline bool GHIInfoMeshVertexLayout::isEmpty() const
{
	return numAttributes == 0;
}

inline const GHIInfoMeshVertexLayout& GHIMesh::getLayout() const
{
	return m_layout;
}

inline std::size_t GHIMesh::numVertexStorages() const
{
	return m_vertexStorages.size();
}

inline const GHIVertexStorage& GHIMesh::getVertexStorage(const std::size_t storageIndex) const
{
	PH_ASSERT_LT(storageIndex, m_vertexStorages.size());
	PH_ASSERT(m_vertexStorages[storageIndex]);
	return *m_vertexStorages[storageIndex].get();
}

inline GHIVertexStorage& GHIMesh::getVertexStorage(const std::size_t storageIndex)
{
	PH_ASSERT_LT(storageIndex, m_vertexStorages.size());
	PH_ASSERT(m_vertexStorages[storageIndex]);
	return *m_vertexStorages[storageIndex];
}

inline bool GHIMesh::hasIndexStorage() const
{
	return m_indexStorage != nullptr;
}

inline const GHIIndexStorage& GHIMesh::getIndexStorage() const
{
	PH_ASSERT(m_indexStorage);
	return *m_indexStorage;
}

inline GHIIndexStorage& GHIMesh::getIndexStorage()
{
	PH_ASSERT(m_indexStorage);
	return *m_indexStorage;
}

}// end namespace ph::editor
