#include "Core/Integrator/Attribute/TAttributeEntry.h"
#include "Common/assertion.h"

namespace ph
{

template<typename Value>
TAttributeEntry<Value>::TAttributeEntry() :
	m_handler(nullptr)
{}

template<typename Value>
TAttributeEntry<Value>::TAttributeEntry(ValueHandler* const handler) :
	m_handler(handler)
{
	PH_ASSERT(m_handler);
}

template<typename Value>
TAttributeEntry<Value>::~TAttributeEntry() = default;

}// end namespace ph