#pragma once

#include "DataIO/Stream/IBinaryDataInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <memory>
#include <istream>

namespace ph
{

class BinaryFileInputStream : public IBinaryDataInputStream
{
public:
	BinaryFileInputStream() = default;
	explicit BinaryFileInputStream(const Path& filePath);
	BinaryFileInputStream(BinaryFileInputStream&& other);

	bool read(std::size_t numBytes, std::byte* out_bytes) override;
	void seekGet(std::size_t pos) override;
	std::size_t tellGet() const override;
	operator bool () const override;

	BinaryFileInputStream& operator = (BinaryFileInputStream&& rhs);

private:
	std::unique_ptr<std::istream> m_istream;
};

// In-header Implementations:

inline BinaryFileInputStream::BinaryFileInputStream(BinaryFileInputStream&& other)
{
	*this = std::move(other);
}

inline BinaryFileInputStream& BinaryFileInputStream::operator = (BinaryFileInputStream&& rhs)
{
	m_istream = std::move(rhs.m_istream);
}

inline BinaryFileInputStream::operator bool () const
{
	return m_istream != nullptr && m_istream->good();
}

}// end namespace ph
