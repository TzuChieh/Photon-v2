#pragma once

#include <cstddef>

namespace ph
{

class IDataStream
{
public:
	virtual ~IDataStream() = default;

	/*virtual void seekPut(std::size_t pos) = 0;
	virtual std::size_t tellPut() const = 0;*/
};

}// end namespace ph
