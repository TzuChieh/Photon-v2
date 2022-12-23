#include "RenderCore/GHIIndexStorage.h"

namespace ph::editor
{

GHIIndexStorage::GHIIndexStorage(const EGHIInfoStorageElement indexType, const EGHIInfoStorageUsage usage)
	: GHIStorage(usage)
	, m_indexType(indexType)
{}

GHIIndexStorage::~GHIIndexStorage() = default;

bool GHIIndexStorage::isIntegerIndexType() const
{
	return 
		m_indexType == EGHIInfoStorageElement::Int8 ||
		m_indexType == EGHIInfoStorageElement::UInt8 ||
		m_indexType == EGHIInfoStorageElement::Int16 ||
		m_indexType == EGHIInfoStorageElement::UInt16 ||
		m_indexType == EGHIInfoStorageElement::Int32 ||
		m_indexType == EGHIInfoStorageElement::UInt32 ||
		m_indexType == EGHIInfoStorageElement::Int64 ||
		m_indexType == EGHIInfoStorageElement::UInt64;
}

}// end namespace ph::editor
