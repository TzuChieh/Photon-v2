#pragma once

#include "DataIO/Stream/IBinaryDataOutputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <memory>
#include <ostream>

namespace ph
{

class BinaryFileOutputStream : public IBinaryDataOutputStream
{
public:
	BinaryFileOutputStream() = default;
	explicit BinaryFileOutputStream(const Path& filePath);
	BinaryFileOutputStream(BinaryFileOutputStream&& other);

	bool write(std::size_t numBytes, const std::byte* bytes) override;
	void seekPut(std::size_t pos) override;
	std::size_t tellPut() override;
	operator bool () const override;

	BinaryFileOutputStream& operator = (BinaryFileOutputStream&& rhs);

private:
	std::unique_ptr<std::ostream> m_ostream;
};

// In-header Implementations:

inline BinaryFileOutputStream::BinaryFileOutputStream(BinaryFileOutputStream&& other)
{
	*this = std::move(other);
}

inline BinaryFileOutputStream& BinaryFileOutputStream::operator = (BinaryFileOutputStream&& rhs)
{
	m_ostream = std::move(rhs.m_ostream);
}

inline BinaryFileOutputStream::operator bool () const
{
	return m_ostream != nullptr && m_ostream->good();
}

}// end namespace ph
