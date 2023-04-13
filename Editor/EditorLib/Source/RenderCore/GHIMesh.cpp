#include "RenderCore/GHIMesh.h"

namespace ph::editor
{

GHIInfoMeshVertexLayout::GHIInfoMeshVertexLayout()
	: attributeBindings()
	, numAttributes(0)
{}

GHIMesh::GHIMesh(
	const GHIInfoMeshVertexLayout& layout,
	TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages)

	: GHIMesh(layout, vertexStorages, nullptr)
{}


GHIMesh::GHIMesh(
	const GHIInfoMeshVertexLayout& layout,
	TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
	const std::shared_ptr<GHIIndexStorage>& indexStorage)

	: m_layout()
	, m_vertexStorages(vertexStorages.begin(), vertexStorages.end())
	, m_indexStorage(indexStorage)
{}

GHIMesh::~GHIMesh() = default;

std::shared_ptr<GHIVertexStorage> GHIMesh::getVertexStorageResource(const std::size_t storageIndex) const
{
	return storageIndex < m_vertexStorages.size()
		? m_vertexStorages[storageIndex]
		: nullptr;
}

std::shared_ptr<GHIIndexStorage> GHIMesh::getIndexStorageResource() const
{
	return m_indexStorage;
}

}// end namespace ph::editor
