#include "RenderCore/GHIVertexStorage.h"

namespace ph::editor
{

GHIVertexStorage::GHIVertexStorage(const GHIInfoVertexLayout& vertexLayout)
	: GHIStorage()
	, m_vertexLayout(vertexLayout)
{}

GHIVertexStorage::~GHIVertexStorage() = default;

}// end namespace ph::editor
