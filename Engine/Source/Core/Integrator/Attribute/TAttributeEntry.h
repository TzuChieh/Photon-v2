#pragma once

#include "Core/Integrator/Attribute/TAttributeHandlerInterface.h"

namespace ph
{

template<typename Value>
class TAttributeEntry
{
public:
	using ValueHandler = TAttributeHandlerInterface<Value>;

	TAttributeEntry();
	TAttributeEntry(ValueHandler* handler);
	virtual ~TAttributeEntry();

	virtual void put(const Value& value) = 0;

private:
	ValueHandler* m_handler;
};

}// end namespace ph

#include "Core/Integrator/Attribute/TAttributeEntry.ipp"