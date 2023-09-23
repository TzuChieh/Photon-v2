#include "RenderCore/Storage.h"

namespace ph::editor::ghi
{

Storage::Storage(const EStorageUsage usage)
	: m_usage(usage)
{}

Storage::~Storage() = default;

}// end namespace ph::editor::ghi
