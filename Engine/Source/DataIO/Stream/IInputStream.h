#pragma once

#include "DataIO/Stream/IDataStream.h"

#include <cstddef>

namespace ph
{

class IInputStream : public IDataStream
{
public:
	virtual bool read(std::size_t numBytes, std::byte* out_bytes) = 0;

	virtual void seekGet(std::size_t pos) = 0;
	virtual std::size_t tellGet() = 0;

	operator bool () const override = 0;

	virtual std::size_t readSome(std::size_t numBytes, std::byte* out_bytes);
};

// In-header Implementations:

inline std::size_t IInputStream::readSome(const std::size_t numBytes, std::byte* const out_bytes)
{
	return read(numBytes, out_bytes) ? numBytes : 0;
}

}// end namespace ph
