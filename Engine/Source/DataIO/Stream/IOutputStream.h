#pragma once

#include "DataIO/Stream/IDataStream.h"

#include <cstddef>

namespace ph
{

class IOutputStream : public IDataStream
{
public:
	virtual bool write(std::size_t numBytes, std::byte* out_bytes) = 0;

	virtual void seekPut(std::size_t pos) = 0;
	virtual std::size_t tellPut() const = 0;

	virtual std::size_t writeSome(std::size_t numBytes, std::byte* out_bytes);
};

// In-header Implementations:

inline std::size_t IOutputStream::writeSome(const std::size_t numBytes, std::byte* const out_bytes)
{
	return write(numBytes, out_bytes) ? numBytes : 0;
}

}// end namespace ph
