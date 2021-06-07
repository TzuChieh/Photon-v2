#pragma once

namespace ph
{

class IDataStream
{
public:
	virtual ~IDataStream() = default;

	virtual operator bool () const = 0;
};

}// end namespace ph
