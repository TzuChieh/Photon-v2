#include "RenderCore/IndexStorage.h"

namespace ph::editor::ghi
{

IndexStorage::IndexStorage(const EStorageElement indexType, const EStorageUsage usage)
	: Storage(usage)
	, m_indexType(indexType)
{}

IndexStorage::~IndexStorage() = default;

bool IndexStorage::isIntegerIndexType() const
{
	return 
		m_indexType == EStorageElement::Int8 ||
		m_indexType == EStorageElement::UInt8 ||
		m_indexType == EStorageElement::Int16 ||
		m_indexType == EStorageElement::UInt16 ||
		m_indexType == EStorageElement::Int32 ||
		m_indexType == EStorageElement::UInt32 ||
		m_indexType == EStorageElement::Int64 ||
		m_indexType == EStorageElement::UInt64;
}

}// end namespace ph::editor::ghi
