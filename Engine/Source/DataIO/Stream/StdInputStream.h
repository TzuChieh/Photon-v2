#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <istream>

namespace ph
{

class StdInputStream : public IInputStream
{
public:
	inline StdInputStream() = default;
	explicit StdInputStream(std::unique_ptr<std::istream> stream);
	StdInputStream(StdInputStream&& other);

	bool read(std::size_t numBytes, std::byte* out_bytes) override;
	void seekGet(std::size_t pos) override;
	std::optional<std::size_t> tellGet() override;
	operator bool () const override;

	std::istream* getStream() const;

	StdInputStream& operator = (StdInputStream&& rhs);

private:
	std::unique_ptr<std::istream> m_istream;

	/*! @brief Enable the use of exceptions when std::istream has error.
	This call will immediately throw @p IOException if the member std::istream
	is already in an error state.
	*/
	void useExceptionForIStreamError();
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
