#include "RenderCore/GHIIndexStorage.h"

namespace ph::editor
{

GHIIndexStorage::GHIIndexStorage(const EGHIStorageElement indexType, const EGHIStorageUsage usage)
	: GHIStorage(usage)
	, m_indexType(indexType)
{}

GHIIndexStorage::~GHIIndexStorage() = default;

bool GHIIndexStorage::isIntegerIndexType() const
{
	return 
		m_indexType == EGHIStorageElement::Int8 ||
		m_indexType == EGHIStorageElement::UInt8 ||
		m_indexType == EGHIStorageElement::Int16 ||
		m_indexType == EGHIStorageElement::UInt16 ||
		m_indexType == EGHIStorageElement::Int32 ||
		m_indexType == EGHIStorageElement::UInt32 ||
		m_indexType == EGHIStorageElement::Int64 ||
		m_indexType == EGHIStorageElement::UInt64;
}

}// end namespace ph::editor
