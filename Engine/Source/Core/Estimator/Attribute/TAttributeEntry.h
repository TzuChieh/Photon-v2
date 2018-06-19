#pragma once

#include "Core/Estimator/Attribute/TAttributeHandlerInterface.h"

namespace ph
{

template<typename Value>
class TAttributeEntry final
{
public:
	using ValueHandler = TAttributeHandlerInterface<Value>;

	TAttributeEntry();
	TAttributeEntry(ValueHandler* handler);

	void put(const Value& value);

	bool hasHandler() const;

private:
	ValueHandler* m_handler;
};

}// end namespace ph

#include "Core/Estimator/Attribute/TAttributeEntry.ipp"