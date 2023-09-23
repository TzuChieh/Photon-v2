#include "RenderCore/VertexStorage.h"

#include <Common/assertion.h>

namespace ph::editor::ghi
{

VertexStorage::VertexStorage(const VertexGroupFormatInfo& format, const EStorageUsage usage)
	: Storage(usage)
	, m_format(format)
{}

VertexStorage::~VertexStorage() = default;

}// end namespace ph::editor::ghi
