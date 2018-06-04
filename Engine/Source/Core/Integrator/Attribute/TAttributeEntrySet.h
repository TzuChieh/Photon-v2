#pragma once

#include "Common/primitive_type.h"
#include "Core/Integrator/Attribute/EAttribute.h"
#include "Core/Integrator/Attribute/TAttributeEntry.h"
#include "Common/assertion.h"

#include <array>
#include <limits>

namespace ph
{

template<typename Value>
class TAttributeEntrySet final
{
public:
	using AttributeEntries = std::array<TAttributeEntry<Value>, static_cast<std::size_t>(EAttribute::SIZE)>;

	TAttributeEntrySet();

	void setEntry(EAttribute type, const TAttributeEntry<Value>& entry);
	void clearEntry(EAttribute type);
	TAttributeEntry<Value>* getEntry(EAttribute type);
	const TAttributeEntry<Value>& getEntry(EAttribute type) const;

private:
	AttributeEntries m_entries;
};

// Implementations:

template<typename Value>
inline TAttributeEntrySet<Value>::TAttributeEntrySet() :
	m_entries()
{}

template<typename Value>
inline void TAttributeEntrySet<Value>::setEntry(
	const EAttribute type, const TAttributeEntry<Value>& entry)
{
	m_entries[static_cast<std::size_t>(type)] = entry;
}

template<typename Value>
inline void TAttributeEntrySet<Value>::clearEntry(const EAttribute type)
{
	m_entries[static_cast<std::size_t>(type)] = TAttributeEntry<Value>();
}

template<typename Value>
inline TAttributeEntry<Value>* TAttributeEntrySet<Value>::getEntry(const EAttribute type)
{
	PH_ASSERT(static_cast<std::size_t>(type) < m_entries.size());

	return &(m_entries[static_cast<std::size_t>(type)]);
}

template<typename Value>
inline const TAttributeEntry<Value>& TAttributeEntrySet<Value>::getEntry(const EAttribute type) const
{
	PH_ASSERT(static_cast<std::size_t>(type) < m_entries.size());

	return m_entries[static_cast<std::size_t>(type)];
}

}// end namespace ph