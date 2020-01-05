#pragma once

#include <cstddef>

namespace ph
{

class IDataStream
{
public:
	virtual ~IDataStream() = default;

	virtual void seekGetPtr(std::size_t pos) = 0;
	virtual void seekPutPtr(std::size_t pos) = 0;
	virtual std::size_t tellGetPtr() const = 0;
	virtual std::size_t tellPutPtr() const = 0;
};

}// end namespace ph
