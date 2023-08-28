#include "RenderCore/GHIVertexStorage.h"

#include <Common/assertion.h>

namespace ph::editor
{

GHIVertexStorage::GHIVertexStorage(const GHIInfoVertexGroupFormat& format, const EGHIStorageUsage usage)
	: GHIStorage(usage)
	, m_format(format)
{}

GHIVertexStorage::~GHIVertexStorage() = default;

}// end namespace ph::editor
