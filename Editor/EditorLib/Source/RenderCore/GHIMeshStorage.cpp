#include "RenderCore/GHIMeshStorage.h"

namespace ph::editor
{

GHIInfoMeshVertexLayout::GHIInfoMeshVertexLayout()
	: attributeBindings()
	, numAttributes(0)
{}

GHIMeshStorage::GHIMeshStorage(
	const GHIInfoMeshVertexLayout& layout,
	TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages)

	: GHIMeshStorage(layout, vertexStorages, nullptr)
{}


GHIMeshStorage::GHIMeshStorage(
	const GHIInfoMeshVertexLayout& layout,
	TSpanView<std::shared_ptr<GHIVertexStorage>> vertexStorages,
	const std::shared_ptr<GHIIndexStorage>& indexStorage)

	: GHIStorage(EGHIInfoStorageUsage::Unspecified)

	, m_layout()
	, m_vertexStorages(vertexStorages.begin(), vertexStorages.end())
	, m_indexStorage(indexStorage)
{}

GHIMeshStorage::~GHIMeshStorage() = default;

std::shared_ptr<GHIVertexStorage> GHIMeshStorage::getVertexStorageResource(const std::size_t storageIndex) const
{
	return storageIndex < m_vertexStorages.size()
		? m_vertexStorages[storageIndex]
		: nullptr;
}

std::shared_ptr<GHIIndexStorage> GHIMeshStorage::getIndexStorageResource() const
{
	return m_indexStorage;
}

}// end namespace ph::editor
