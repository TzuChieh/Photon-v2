#include "RenderCore/Mesh.h"

namespace ph::editor::ghi
{

MeshVertexLayoutInfo::MeshVertexLayoutInfo()
	: attributeBindings()
	, numAttributes(0)
{}

Mesh::Mesh(
	const MeshVertexLayoutInfo& layout,
	TSpanView<std::shared_ptr<VertexStorage>> vertexStorages)

	: Mesh(layout, vertexStorages, nullptr)
{}


Mesh::Mesh(
	const MeshVertexLayoutInfo& layout,
	TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
	const std::shared_ptr<IndexStorage>& indexStorage)

	: m_layout()
	, m_vertexStorages(vertexStorages.begin(), vertexStorages.end())
	, m_indexStorage(indexStorage)
{}

Mesh::~Mesh() = default;

std::shared_ptr<VertexStorage> Mesh::getVertexStorageResource(const std::size_t storageIndex) const
{
	return storageIndex < m_vertexStorages.size()
		? m_vertexStorages[storageIndex]
		: nullptr;
}

std::shared_ptr<IndexStorage> Mesh::getIndexStorageResource() const
{
	return m_indexStorage;
}

}// end namespace ph::editor::ghi
