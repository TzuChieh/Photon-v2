#include "RenderCore/GHIStorage.h"

namespace ph::editor
{

GHIStorage::GHIStorage(const EGHIStorageUsage usage)
	: m_usage(usage)
{}

GHIStorage::~GHIStorage() = default;

}// end namespace ph::editor
