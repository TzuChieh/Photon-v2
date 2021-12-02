#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <istream>
#include <memory>

namespace ph
{

class StdInputStream : public IInputStream
{
public:
	inline StdInputStream() = default;
	explicit StdInputStream(std::unique_ptr<std::istream> stream);
	StdInputStream(StdInputStream&& other);

	void read(std::size_t numBytes, std::byte* out_bytes) override;
	void seekGet(std::size_t pos) override;
	std::optional<std::size_t> tellGet() override;
	operator bool () const override;
	std::size_t readSome(std::size_t numBytes, std::byte* out_bytes) override;

	std::istream* getStream() const;

	StdInputStream& operator = (StdInputStream&& rhs);

protected:
	/*! @brief Check if the stream has any error.
	@exception IOException if there is any error.
	*/
	void ensureStreamIsGoodForRead() const;

private:
	std::unique_ptr<std::istream> m_istream;
};

// In-header Implementations:

inline StdInputStream::StdInputStream(StdInputStream&& other)
{
	*this = std::move(other);
}

inline StdInputStream& StdInputStream::operator = (StdInputStream&& rhs)
{
	m_istream = std::move(rhs.m_istream);
}

inline StdInputStream::operator bool () const
{
	return m_istream != nullptr && m_istream->good();
}

inline std::istream* StdInputStream::getStream() const
{
	return m_istream.get();
}

}// end namespace ph
