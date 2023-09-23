#pragma once

#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Utility/TSpan.h>

#include <array>
#include <memory>
#include <vector>
#include <cstddef>

namespace ph::editor::ghi
{

class VertexStorage;
class IndexStorage;

class MeshVertexLayoutInfo final
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
	MeshVertexLayoutInfo();

	bool isEmpty() const;
};

class Mesh
{
public:
	Mesh(
		const MeshVertexLayoutInfo& layout,
		TSpanView<std::shared_ptr<VertexStorage>> vertexStorages);

	Mesh(
		const MeshVertexLayoutInfo& layout,
		TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
		const std::shared_ptr<IndexStorage>& indexStorage);

	virtual ~Mesh();

	virtual void bind() = 0;

	const MeshVertexLayoutInfo& getLayout() const;

	std::size_t numVertexStorages() const;
	const VertexStorage& getVertexStorage(std::size_t storageIndex) const;
	VertexStorage& getVertexStorage(std::size_t storageIndex);
	std::shared_ptr<VertexStorage> getVertexStorageResource(std::size_t storageIndex) const;

	bool hasIndexStorage() const;
	const IndexStorage& getIndexStorage() const;
	IndexStorage& getIndexStorage();
	std::shared_ptr<IndexStorage> getIndexStorageResource() const;

private:
	MeshVertexLayoutInfo m_layout;
	std::vector<std::shared_ptr<VertexStorage>> m_vertexStorages;
	std::shared_ptr<IndexStorage> m_indexStorage;
};

inline bool MeshVertexLayoutInfo::isEmpty() const
{
	return numAttributes == 0;
}

inline const MeshVertexLayoutInfo& Mesh::getLayout() const
{
	return m_layout;
}

inline std::size_t Mesh::numVertexStorages() const
{
	return m_vertexStorages.size();
}

inline const VertexStorage& Mesh::getVertexStorage(const std::size_t storageIndex) const
{
	PH_ASSERT_LT(storageIndex, m_vertexStorages.size());
	PH_ASSERT(m_vertexStorages[storageIndex]);
	return *m_vertexStorages[storageIndex].get();
}

inline VertexStorage& Mesh::getVertexStorage(const std::size_t storageIndex)
{
	PH_ASSERT_LT(storageIndex, m_vertexStorages.size());
	PH_ASSERT(m_vertexStorages[storageIndex]);
	return *m_vertexStorages[storageIndex];
}

inline bool Mesh::hasIndexStorage() const
{
	return m_indexStorage != nullptr;
}

inline const IndexStorage& Mesh::getIndexStorage() const
{
	PH_ASSERT(m_indexStorage);
	return *m_indexStorage;
}

inline IndexStorage& Mesh::getIndexStorage()
{
	PH_ASSERT(m_indexStorage);
	return *m_indexStorage;
}

}// end namespace ph::editor::ghi
