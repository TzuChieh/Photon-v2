#pragma once

#include "DataIO/Stream/IDataStream.h"

#include <cstddef>

namespace ph
{

class IInputStream : public IDataStream
{
public:
	virtual std::size_t read(std::size_t numBytes, std::byte* out_bytes) = 0;

	virtual void seekGet(std::size_t pos) = 0;
	virtual std::size_t tellGet() const = 0;
};

}// end namespace ph
