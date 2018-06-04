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
void TAttributeEntry<Value>::put(const Value& value)
{
	PH_ASSERT(m_handler);

	m_handler->handleSinglePut(value);
}

template<typename Value>
bool TAttributeEntry<Value>::hasHandler() const
{
	return m_handler;
}

}// end namespace ph