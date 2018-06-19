#pragma once

namespace ph
{

template<typename Value>
class TAttributeHandlerInterface
{
public:
	virtual ~TAttributeHandlerInterface() = default;

	virtual void handleSinglePut(const Value& value) = 0;
};

}// end namespace ph