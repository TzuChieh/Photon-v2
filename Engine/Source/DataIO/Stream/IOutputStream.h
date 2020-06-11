#pragma once

#include "DataIO/Stream/IDataStream.h"

#include <cstddef>

namespace ph
{

class IOutputStream : public IDataStream
{
public:
	virtual bool write(std::size_t numBytes, const std::byte* bytes) = 0;

	virtual void seekPut(std::size_t pos) = 0;
	virtual std::size_t tellPut() = 0;

	operator bool () const override = 0;

	virtual std::size_t writeSome(std::size_t numBytes, const std::byte* bytes);
};

// In-header Implementations:

inline std::size_t IOutputStream::writeSome(const std::size_t numBytes, const std::byte* out_bytes)
{
	return write(numBytes, out_bytes) ? numBytes : 0;
}

}// end namespace ph
